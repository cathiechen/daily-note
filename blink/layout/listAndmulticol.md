# list-multi-column-crash

一切源自`list-multi-column-crash`通不过。`MinimumSpaceShortageFinder::ExamineLine`里面的DECHECK通过不了。一脸懵逼啊！这个DECHECK是啥意思？自己产生的crash，而且还发了cr，跪着也要查下去。几乎真的是跪着查完的。搭环境，看mc逻辑，花了一周多的时间才算整明白。

先来看看mc是怎样排版的。

## mc的排版算法

先看下，这个crash的例子。
```
     LayoutBlockFlow 0x25971be18260   	DIV class="columns"
        LayoutMultiColumnFlowThread (anonymous) 0x25971be24010
          LayoutListItem 0x25971be34010	P class="list"
            LayoutBlockFlow (anonymous) 0x25971be18950
              LayoutListMarker (anonymous) 0x25971be18388
            LayoutBlockFlow 0x25971be18700	HR
              LayoutBlockFlow 0x25971be185d8	DIV class="columns"
                LayoutMultiColumnFlowThread (anonymous) 0x25971be241b0
            LayoutBlockFlow (anonymous) 0x25971be184b0
              LayoutText 0x25971be500e0	#text "Text."
        LayoutMultiColumnSet (anonymous) 0x25971be44010
```
mc分成两部分LayoutMultiColumnFlowThread和LayoutMultiColumnSet。从layout tree中可以看到LayoutMultiColumnFlowThread拥有了所有的孩子节点，拥有自己独自的坐标系，孩子节点根据自己距离LayoutMultiColumnSet的距离，决定该孩子节点所处column。LayoutMultiColumnSet没有孩子节点，但它决定了column的高度。layout会有很多遍，具体触发的地方在：`LayoutBlockFlow::UpdateBlockLayout`
```
    if (flow_thread && !flow_thread->FinishLayout()) {
      SetChildNeedsLayout(kMarkOnlyThis);
      continue;
    }
```
这个会触发LayoutMultiColumnFlowThread重新layout一次。

因为，这次跟的是高度，所以，先介绍下高度如何处理。

1. 第一次layout，像layout block一样layout LayoutMultiColumnFlowThread，基本没有什么特殊处理。在layout LayoutMultiColumnSet时，会去计算column height。具体`LayoutMultiColumnSet::RecalculateColumnHeight`。其实LayoutMultiColumnSet很多工作都会转到`MultiColumnFragmentainerGroup`处理。`MultiColumnFragmentainerGroup`是一群拥有相同高度的column集合，其实上就是一堆对应LayoutMultiColumnFlowThread的偏移，column的高度等，有了这些数据，就可以根据偏移量计算出该offset所处column。顾名思义，`LayoutMultiColumnSet::RecalculateColumnHeight`会计算一下column的高度。计算逻辑`MultiColumnFragmentainerGroup::RecalculateColumnHeight`。第一次过来的时候，会创建`InitialColumnHeightFinder`，并通过`InitialMinimalBalancedHeight`确定高度，其实就是LayoutMultiColumnFlowThread 的layout overflow / column count。InitialMinimalBalancedHeight还会traverse所有的孩子，计算column最小应该有的高度。如果，column height发生变化，就会设置`LayoutMultiColumnFlowThread的SetColumnHeightsChanged`（决定是否finishlayout）。所以，肯定会触发第二次排版。

2. 第二次layout： layout LayoutMultiColumnFlowThread及其孩子是会多出一些事情，那就是添加paginate。主要有：`LayoutBlockFlow::AdjustBlockChildForPagination`和`LayoutBlockFlow::AdjustLinePositionForPagination`。
	- `LayoutBlockFlow::AdjustBlockChildForPagination`计算由于column 所剩的高度不够排下block child的，所以需要往该child前面添加一个`pagination_strut`，让child可以排到下一个column。`pagination_strut`会被保存layoutbox中，但每次layout都会重新计算。
	- `LayoutBlockFlow::AdjustLinePositionForPagination`是在rootlinebox的层面计算`pagination_strut`，这个会被保存在rootinlinebox中。
	- 判断LayoutMultiColumnFlowThread的哪些孩子需要重新layout？决定权在：`LayoutBox::MarkChildForPaginationRelayoutIfNeeded`，主要是判断该孩子是否有跨column的情况，若有，就要重新排一下。通过孩子节点overflow height和remaining height（当前column剩下的高度）对比。有没有可能出现child加了`pagination_strut`，但此时不会跨column的情况？
到了LayoutMultiColumnSet，继续`RecalculateColumnHeight`。此时`MultiColumnFragmentainerGroup::RecalculateColumnHeight`会创建MinimumSpaceShortageFinder。（之前，会判断是否多出了column，logical height是否比max还大） MinimumSpaceShortageFinder大概就是寻找一个最小的shortage，把这个shortage加到column height中。
	- 寻找最小的shortage。`ColumnBalancer::Traverse()`遍历所有孩子。
		- `ColumnBalancer::TraverseChildren`中每个节点，在遍历孩子节点之前会先`MinimumSpaceShortageFinder::ExamineBoxAfterEntering`。对于`IsFirstAfterBreak(FlowThreadOffset())`，也就是第二列或以后位于列首的child，利用之前计算出来的`PaginationStrut`计算该child实际缺少的高度，也就是：`child_logical_height - strut`, 利用`RecordSpaceShortage`记录下来，就是找个最小的先。对于跨列的child，也会记录，大小是child的`bottom_in_flow_thread - group.ColumnLogicalTopForOffset(bottom_in_flow_thread)`，也就是跨到下一列的高度。
 		- `ColumnBalancer::TraverseLines`处理rootinlinebox层面的shortage。对每行`ExamineLine`，看下下面的DECHECK的意思：1. 该行位于列首的位置 2. line中没有设置`pagination_strut` 3. 除去`pagination_strut`行首的位置不在LayoutMultiColumnFlowThread对应区域内。是有道理的，rootinlinebox要么命中1 3，要么不该有`pagination_strut`。（最新计算出来的`pagination_strut`会满足以上的一种情况，若过期的，将会引起DECHECK通不过）。列首的inlinebox，记录所缺的shortage（`line_height - line.PaginationStrut()`）。对跨列的行，记录shortage（`line_bottom` - 后一列列首位置）。
		```
		  DCHECK(IsFirstAfterBreak(line_top_in_flow_thread) ||
         		!line.PaginationStrut() ||
         		!IsLogicalTopWithinBounds(line_top_in_flow_thread -
                                   		line.PaginationStrut()));
		```
添加所计算出来的最小shortage到column height。若column height发生变化，设置`LayoutMultiColumnFlowThread的SetColumnHeightsChanged`。触发下一次layout

3. 第三次layout：跟第二次一样，但“判断LayoutMultiColumnFlowThread的哪些孩子需要重新layout”要正确，不然layout不触发，就不会更新上次的`pagination_strut`了。会导致DECHECK错误。

4. 什么时候结束？不再产生多余要求的column。column height没变

## list-multi-column-crash crash的原因

由于`marker_container`（LayoutBlockFlow (anonymous) 0x25971be18950）的overflow没有考虑到marker垂直方向的移动，所以y pos一直是0. 导致`marker_container`被判断成不需要重新layout的节点 ，rootinlinebox的`pagination_strut`没有被更新，最终在第三次layout的时候，`ExamineLine`的DECHECK通不过。

## 重要的函数

1.  `LayoutBlockFlow::UpdateBlockLayout`触发下一次layout的点。知道了，这样才有坐标。

```
    if (flow_thread && !flow_thread->FinishLayout()) {
      SetChildNeedsLayout(kMarkOnlyThis);
      continue;
    }
```

2. `MultiColumnFragmentainerGroup::RecalculateColumnHeight` 中的`SetAndConstrainColumnHeight(new_column_height);`。这样才能知道column height变成多少了

3. `LayoutBlockFlow::AdjustBlockChildForPagination`计算当前情况下box的`pagination_strut`。会影响LayoutMultiColumnFlowThread 的height。

4. `LayoutBlockFlow::AdjustLinePositionForPagination` 同上

5. MultiColumnFragmentainerGroup的概念很重要。感受一下

```
(const blink::MultiColumnFragmentainerGroup &) @0x2d71018d0130: {column_set_ = @0x2d71018d0010, logical_top_ = {value_ = 0}, logical_top_in_flow_thread_ = {value_ = 0},
  logical_bottom_in_flow_thread_ = {value_ = 3840}, logical_height_ = {value_ = 1920}, max_logical_height_ = {value_ = 2147483647}, is_logical_height_known_ = true}
```
6. ColumnBalancer其实就是为了计算column 高度做准备。所有遍历都只是为了RecordSpaceShortage就是找个最小值。

## 感悟

本来以为没有自己完全没有头绪的layout算法了。之前还是有看过multicolumn的设计文档的，很好，翻译记录下来了。但各种layout pass还是被绕晕。但，最后绕出来，觉得这个算法真是又复杂又美丽。从一开始各种触发排版的地方就头晕，到现在，挺有成就感的。

看代码的过程中，还是太过于注重流程，先以类为基础再缕逻辑，应该会简单一点。
还有，看代码也应该有笔记，不然一个下午，真的不知道看了什么。
太依赖gdb，gdb的一个毛病就是，看不到全局。
打log很重要。有gdb也要打log啊。

就酱！
