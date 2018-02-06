# LayoutNG

[LayoutNG]("https://chromium.googlesource.com/chromium/src/+/master/third_party/WebKit/Source/core/layout/ng/README.md")的翻译。

## 总体
layout mode由display控制（还有custom layout），不同类型的layout有不同的NGLayoutAlgorithm.
layout需要一下参数：
1. NGBlockNode：这个代表要layout的节点。
	1.1 该节点的style
	1.2 该节点的孩子列表
2. NGConstraintSpace：代表layout产生NGPhysicalFragment的空间
3. BreakTokens：每次layout完一个节点都应该走到BreakTokens中
除此之外，layout不应该再访问其它信息。

## Fragment Caching
layout之后，会cache结果，以免下次需要重做layout。
1. 在LayoutNGBlockFlow中，保存constraint space 和 resulting fragment。为了简化，只有在没有break tonken的情况下才保存。保存结果接口：LayoutNGBlockFlow::SetCachedLayoutResult
2. 若已cached，layout之前可以取到值，layout就好了。通过这个接口：LayoutNGBlockFlow::CachedLayoutResult
3. CachedLayoutResult会clone fragment，但没有offset。这样父亲节点就可以定位这个fragment了
4. 只有满足下列条件才会重用：
	4.1 没有break token
	4.2 不需要重排的节点，还是needslayout。

---
---
---

## 读代码笔记
最近看代码老走神！！！TT
在LayoutNG中，把layout result序列化为Fragment，相应的数据结构有：
- `NGPhysicalFragment`
  - `NGPhysicalTextFragment`
  - `NGPhysicalContainerFragment`
    - `NGPhysicalLineBoxFragment`
    - `NGPhysicalBoxFragment`
把这些结果保存到layoutobject中`block_flow->SetCachedLayoutResult`.

如何唯一标识一个framgment：
- `NGLayoutInputNode`
- `NGBreakToken`
- `NGConstraintSpace`

如何layout：
layout是通过`NGLayoutAlgorithm`，这个类对外只提供两个函数：`Layout()`和`ComputeMinMaxSize()`.
不同的node调用不同的`NGLayoutAlgorithm`，其子类有：
- `NGInlineLayoutAlgorithm`
- `NGBlockLayoutAlgorithm`
- `NGColumnLayoutAlgorithm`
- `NGPageLayoutAlgorithm`
根据`NGLayoutInputNode`的类型来区分不同的是排版算法：
- `NGInlineNode`
- `NGBlockNode`
这些node是在`LayoutNGBlockFlow`等创建的。创建完，layout之后，这些node就删了？

如何从LayoutObject过渡到NG：
`LayoutNGBlockFlow`等继承`LayoutNGMixin<LayoutBlockFlow>`，其实就是继承`LayoutBlockFlow`。


一些NG中使用的单位：
- NGPhysicalBoxStrut 物理上的，不依赖于write mode。left、right就是屏幕的左右，top、bottom就是屏幕的上下
- NGBoxStrut 逻辑上的，依赖于write mode。所以，一般就是`inline_xx`和`block_xx`
带logical的就是依赖于write mode的，带physical的就是不依赖write mode的。


重要的数据结构和概念：
- `NGConstraintSpace`：里面包含所有layout需要的信息，如：`available_size`, `margin_strut`，`unpositioned_floats_`等
- `NGMarginStrut`：用于 margin collapse的计算
- `NGUnpositionedFloat`：包含定位float的信息，包含float node， available size等信息
- `bfc_offset_`：当前fragment放置于bfc的位置
- `bfc`：block formatting context， 独立排版的一个空间，也是某个node？
- `NGBlockBreakToken`：中断信息，包含`child_break_tokens_`和`used_block_size_`等重要信息。
- `NGPhysicalFragment`：是layout result，包含：layoutobject, style, NGPhysicalSize, NGPhysicalOffset, NGBreakToken
- `NGPhysicalTextFragment`: `NGPhysicalFragment`的子类，包含：text，相对与parent block的`start_offset_` 和 `end_offset_`等
- `NGPhysicalContainerFragment`：`NGPhysicalFragment`的子类，包含：`child NGPhysicalFragment`和`contents_visual_rect_`
- `NGPhysicalBoxFragment`：`NGPhysicalContainerFragment`的子类，包含：`NGBaseline`
- `NGPhysicalLineBoxFragment`：`NGPhysicalContainerFragment`的子类，包含:`NGLineHeightMetrics`
- `NGFragment`, `NGLineBoxFragment`, `NGTextFragment`, `NGBoxFragment`：对应于相关physical的logicalfragment，考虑write mode。
- `NGBaseFragmentBuilder`：收集layout过程中的数据，包含：style、write mode、direction，然后用`ToTextFragment()`,`ToLineBoxFragment()`等转化成相应的fragment
  - `NGTextFragmentBuilder`：
  - `NGContainerFragmentBuilder`：
    - `NGLineBoxFragmentBuilder`：
    - `NGFragmentBuilder`：这个命名不怎么谨慎吧？
- `NGLayoutResult`：layout result，包含：`NGPhysicalFragment`，position，float等信息，应该是layout对外结果的一层封装。


TODO:
学习下各种ptr，Optional等。。




# LayoutNG Block Layout

- formatting context: 代表css的display
- block formatting context: 不是所有的block都创建new bfc。以下情况创建：overflow：hidden; root; flex；grid
- NGPhysicalFragment：（aka.fragment）：包含layout产生的所有信息，可恩那个有些node有多个fragment，如：multi-column
- NGLayoutResult:包含fragment和其他layout需要的信息。
- NGConstraintSpace：包含所有来自parents的信息：available space; percentage fragmentation line？
- NGExclusionSpace： float相关，float所有的logic都封装在里面。
- NGLayoutInputNode，NGBlockNode，NGLineNode：通过调用layout获得fragment，基本上是const layout object的一个封装。
- NGBreakToken：用于把layout函数传给调用函数，用于告诉child从某个断点开始恢复。

基本原则：LayoutNG的input（NGConstraintSpace）和output（LayoutResult）都应该是const的。
margin collapse：公式：max positive margin + min negative margin

```
NGMarginStrut{
    positive_margin;
    negative_margin;
};

```
NGConstraintSpace有一个NGMarginStrut输入;
LayoutResult有一个NGMarginStrut输出。

NGBfcOffset：相对于parent bfc的offset。
一个node的NGBfcOffset遇到非空内容时，才能被resolved
