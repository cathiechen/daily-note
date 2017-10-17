# marker定位重构

## 目的
在现有逻辑中，outside marker被加到li的孩子节点中，这样会导致一些问题。本文档的目的是把outside marker变成li的直接孩子，再调整其垂直方向上的位置，让其位置正确，且不影响li的孩子节点。

## 现状
现在，marker是一个inline atomic的元素。它的位置根据`list-style-position`分成outside和inside。本文档讨论outside。目前，代码中对与outside的定位方法为：
1. 把marker添加为`GetParentOfFirstLineBox`的第一孩子节点。
	- `GetParentOfFirstLineBox`： 获取li孩子节点中第一个产生inlinebox的节点。
2. 处理marker在水平方向上的位置。

这样做存在的问题： 
1. marker对li孩子节点的影响：marker的css属性是继承与li的，如果把marker加入到li的child node中，有可能会影响孩子的显示。如：
	```
	  <li style="font-size: 50px">
	    <div style="font-size: 10px">xxx</div>
	  </li>
	```
上面的例子中，div的高度将受到marker的影响，变成50px！

2. li的孩子节点对marker的显示的影响，如：
	```
	  <li>
	    <div style="overflow: hidden">xxx</div>
	  </li>
	```
上面的例子中，在CR修改之前，marker被overflow hidden掉，无法显示出来；在crxxx后，可以显示，但会导致换行，这也是我们不想看到的。

## ZeroHeight的解决方法的总体思路
通过对现状的分析，我们可以看到，把marker加到li的孩子节点里面引起的问题。本方案把marker直接加为li的孩子。若li的孩子是block的，marker将独自占用一行，这里通过设置marker container的logicalHeight 0px解决该问题。下面是marker添加到layout tree及其position的流程：
1. 添加marker到li中 ，确保marker是li的直接孩子节点，或者若产生匿名块marker container，marker将是其的唯一孩子（改变）
2. 如果存在marker container，设置marker container的高度为0px。这样可以阻止marker产生一个新的行。（新增逻辑）
3. block方向上对齐marker和li 的第一个inline box （新增逻辑）
4. 处理marker在inline方向的位置

## details
### marker所在dom树位置
确保marker是li的直接孩子节点，或者，确保marker是marker container的唯一孩子。为了确保marker的独立性，marker不应该与其他节点共享一个匿名块parent。

1. li的孩子节点是inline： 直接加入li
```
<li><span>text</span></li>
```
layout tree:
```
LayoutListItem       li
  LayoutListMarker
  LayoutInline       span
    LayoutText       "text"
```
2. li的孩子节点是block： 创建一个ab作为marker container，并把marker加到marker container中。marker container加到li中，作为li的第一个孩子节点    
```
<li><div>text</div></li>
```
layout tree:
```
LayoutListItem       li
  LayoutBlock        anonymous, marker container
    LayoutListMarker
  LayoutBlock        div
    LayoutText       "text"
```
3. 更新marker的位置：若layout tree发生变化，有新的节点加到marker container中，将marker移出，设置marker container的高度为li的高度，再为marker创建新的marker container，并加到li中。这样可以确保同一个dom树，生成的layout tree是一样的，无论dom是否发生变化。
```
<li></li>
```
li的孩子节点为空，直接加入marker。
layout tree:
```
LayoutListItem       li
  LayoutListMarker
```
dom 树发生变化：
```
<li>
  <span></span>
</li>
```
span作为inline节点，直接加到marker背后，marker的layout tree位置不需要改动。
layout tree：
```
LayoutListItem       li
  LayoutListMarker
  LayoutInline       span
```
```
<li>
  <span><div>xxx</div></span>
</li>
```
为marker和“first part of span”创建匿名块parent。设置匿名块的logicalHeight为li的height，remove marker，为marker创建自己的marker container，把marker container加到li中。
```
LayoutListItem       li
  LayoutBlock        anonymous, marker container
    LayoutListMarker
  LayoutBlock        anonymous
    LayoutInline     first part of span
  LayoutBlock  	     anonymous
    LayoutBlock      div
  LayoutBlock        anonymous
    LayoutInline     second part of span
```

### marker container的logicalHeight管理

这里，决定marker container的高度。
1. 对于`GetParentOfFirstLineBox`是null的li，marker container 不会被设置成0px。`GetParentOfFirstLineBox() == nullptr`代表该li中存在特殊的情况（如：nested list），需要换行，所以此时不改变height。
2. marker container中只包含marker一个孩子节点，设置marker container的高度为0px。marker需要垂直对齐，marker不应该占用高度，故设置成0px。
3. marker container中包含出marker以外的孩子节点，设置marker container的高度为li的高度。（marker container和li公用一个style）这种情况需要重新创建marker container，所以应该还原原来的高度。

### marker和li的first line box的block direction对齐方案

1. 通过`GetParentOfFirstLineBox`获取到marker需要对齐的行`line_box_parent`，并获得其rootinlinebox：`line_box_parent_root`
2. 计算需要移动marker的偏移量
	2.1 通过`line_box_parent_root`计算出
		2.1.1 marker的logicalTop = top + maxAscent - marker的fontmetric.ascent. (top = `line_box_parent`的LogicalHeight)
		2.1.2 `line_box_parent_root`的logicalTop = top + maxAscent - `line_box_parent_root`的fontmetric.ascent.
		2.1.3 marker的logicalTop = top + `line_box_parent_root`的fontmetric.ascent - marker的fontmetric.ascent。
	2.2 处理祖先节点和当前logicalTop的偏移量:offset = marker的logicalTop + line_box_parent block offset - marker old logicalTop - marker block offset。
3. 垂直方向上移动marker的`InlineBoxWrapper` offset.
具体见代码。

### marker和margin collapse

由于marker被添加为li的第一个孩子，这样会导致margin collapse的异常，如：

```
<li style="margin-top:10px;">
  <div style="margin-top:20px">text</div>
</li>
```
此处应该由个margin collapse，div的logicalTop最终是20px。但由于marker作为第一个孩子，layout tree变成
```
<li style="margin-top:10px;">
  <anonymous block>marker<anonymous block>
  <div style="margin-top:20px">text</div>
</li>
```
由于marker的存在，这会阻碍li和div的margin collapse。
所以为了解决上述问题，在生成margin collapse的过程中，需要对marker做一些处理。
1. 去除marker container对`LayoutBlockFlow::MarginBeforeEstimateForChild`的影响。
2. 不应该因为marker container而改变`margin_info.SetAtBeforeSideOfBlock(false);`
具体位置请看代码。

## 其他
我们还考虑过把outside marker的位置变成absolute的方案，这个方案存在的问题：
1. 若li的位置变化了，将不会通知absolute的marker去更新自己的位置。
2. absolute的container不好确定，对齐方式将更复杂。
