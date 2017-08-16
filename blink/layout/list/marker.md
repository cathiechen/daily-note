# 重构marker position的实现


## W3C中关于listMarker的规定

- 现有规定：
- 后续可能添加的规定：


## 现有方案

- `ListMarker`都是inline的
- `LayoutListItem::SubtreeDidChange()`时，调用`LayoutListItem::UpdateMarkerLocation()`;
  - 确定`line_box_parent`
    - 第一个line_box的父亲节点
    - `<li>`
  - remove marker from 之前的节点
  - `line_box_parent->AddChild(marker_, FirstNonMarkerChild(line_box_parent))`
  - `marker_->UpdateMarginsAndContent();`更新marker的content和margin(偏移设置在margin里面)
    - `LayoutListMarker`的layout
  - 除了计算宽高，还有计算一个值： `line_offset_`
  - `line_offset_`: `ListItem()->LogicalLeftOffsetForLine(block_offset, kDoNotIndentText, LayoutUnit());`= float object宽度 + `TextIndentOffset()`
- 在`LayoutBlockFlow::UpdateBlockLayout()`时，最后会`ComputeOverflow(unconstrained_client_after_edge);`
  - `AddOverflowFromChildren();`， 在`LayoutListItem::AddOverflowFromChildren()`还要`PositionListMarker();`
  - `PositionListMarker();`
    - 移动`marker_logical_left`, `marker_->InlineBoxWrapper()->MoveInInlineDirection()`, 里面包含`line_offset_`
    - 然后处理overflow的事情: 
      - marker的InlineFlowBox，以及其parent，OverrideOverflowFromLogicalRects, 重新计算的layoutrect和visionrect.
      - block level, ParentBox(), 向上传递overflow rect
- 总结：
  -  创建一个inline的marker
  - 把marker加上layout tree，并更新marker的margin和content
  - computeoverflow（li也就是layout完所有孩子后），重新更正marker的水平位置
  - 这个方案是：把marker加入content flow里，省略了垂直位置的的计算，只要计算水平位置，但存在以下问题
    - !overflow: visibility		
    - nested li
    - 空content高度
    - ...



## 新方案

- inside的marker作为inline； outside的marker作为absolute
  - absolute需要一个container，所以还是要（0,0,0,0）的relative container
  - 更新后不要管水平方向的位置变化，只要管垂直方向的，那line-height这个怎么算？
  - inside和outside互换的时候如何处理？

## questions

- 嵌套的`<li><ul><li></li></ul></li>`是否应换行？
	- old: 换行
	- firefox：不换行
- 没有li作为父亲节点的marker怎么办？
	- 若是通过`::marker`定义的marker，则跟relative一样
	- 原来的方式没办法独立设置marker的
- 如果li的孩子节点有负值的孩子，marker应该跟着偏移吗？还是与兄弟marker对齐
	- old: 不偏移
	- firefox: 不偏移
- line-height很大的内容的marker应跟内容对齐
	- old： 除了overflow和nested li，其它对齐
	- firefox: 都对齐
- 如果li的孩子内容是空的，是否该由marker支撑起高度
	- old: 支撑
	- firefox： 不支撑
- inside 和 outside 切换



## [css-lists-3翻译](https://www.w3.org/TR/css-lists-3/)

### 1.本文档介绍的内容： 
- `::marker`
- `list-item` 和 `inline-list-item`
- `pisition: marker`
- 一些控制marker的定位和style的属性
- counter


### 2. 声明一个list item
List Item是display被设置成`list-item` 和 `inline-list-item`的元素。只有list item 自动生成`::marker`伪类元素，其它元素不会自动生成。

另外，li的counter是默认增加的。除非通过`counter-increment`设置成递减1，否则，都是默认增加1的。

新增`display: inline-list-item`: `inside`的li。除此之外，跟inline没差。


### 3. 默认的marker内容：`list-style-image`和`list-style-type`属性
marker的自动生成内容由这两个属性控制。如果没设置`list-style-image`或image不是一个有效值，内容由`list-style-type`控制。

`list-style-style`可以设置成：conter-style|string|none.


### 4. marker定位：`list-style-position`属性
可以设置成： inside和outside，用于控制marker的位置。

- inside：marker是*inline元素*，定位于`::before`前面
- outside：把`position`的值加到marker上面。另外，marker的文字排版方向必需与“marker positioning reference element”一样

若`display: inline-list-item`则是`inside`
注意：只有marker的内容不是空的时候才会产生marker.


### 5. `list-style` shorthand 属性
三种属性一起设置：`list-style-type`，`list-type-position`和`list-style-image`
none的歧义问题：type和image都没设置，则两个都是none;否则，none应用于设置了值之外的那个属性。

### 6. markers: `::marker`伪类

新增的伪类`::marker`, 由li生成，并作为其marker。
`::marker`生成的是一个element，可以应用css。marker定位与`::before`的前面，默认是inline的。然而，`list-style-position`的一些特殊设置，可以让marker box变成positioned，这个会影响display属性
这个伪类只能作用于li，其它的无效。

#### 6.1. 生成内容的content属性
`::marker`的内容由`list-style-type`和`list-style-image`决定

### 7. 定位markers
介绍新的position架构，用于模拟css2.1中定义的list marker(outside)的定位行为。这个`position: marker`可以用于所有元素。可以让任何元素作为li的marker，不管css是否应用到，该元素的定位位置都跟native的marker是一样的。（这个定义可以使marker更多样化）

#### 7.1 `position: marker`
`marker`是否有效，取决于是否存在li祖先节点。若无li祖先节点，position值被设置成`relative`。 若有，`position: marker`当作*absolutely positioned*(所以，是自己有个computelogicalposition, 不用css计算，哇。。有点激动)

如何定位，先解释以下terms：
- ancestor list item(ALI): marker最近的li祖先节点
- mark postioning reference element(MPRE): 
	```
	if (ALI's marker-side:list-item)
	  MPRE = ALI;
	else if (ALI's marker-side:list-container && ALI has parent)
	  MPRE = ALI's parent;
	else
	  MPRE = ALI;
	```
- list item positioning edge(LIPE): ALI的开始排版的边缘与MPRE的一样，不管ALI的坐标是怎样的。
- marker positioning edge(MPE): marker的外部边缘与ALI的相反，即：ALI的左边缘和marker的右边缘一样

marker的block方向位置跟normal flow的element一样。
inline方向上，marker的位置，必须保证，MPE和LIPE齐平。

注意：这么绕的定义，是为了使marker和li齐平。然后让`marker-side:list-container`可以定义所有marker在li的同一边（即使li们可能有不同的`dir`），这样定义一遍的padding就可以把所有marker显示出来。基于这个实现，可以让`marker`, `li`和`container`有不同的`dir`的时候，看起来合理一点。

一个li可以有多个marker，top, bottom, left, right可以设置他们的相对位置（跟relative一样）

example： `position: marker`可以用于，只有marker的内容比较重要，而style不那么重要的环境

#### 7.2 `marker-side`属性
默认情况下，marker的位置是根据li的`dir`的。然后，若li们有不同的`dir`属性时，为了让所有marker都显示在li的同一边，此时，可以使用`marker-side`
有两个值:
- list-item: 默认值，以li的dir为准
- list-container: 以li的父亲节点的dir为准。

### 8. counter相关的没翻



## [CSS2.2 lists翻译](https://www.w3.org/TR/CSS22/generate.html#listso)

CSS2.2 为li的content创建一个主块，根据`list-style-typ`和`list-style-image`的值，会创建一个marker box，以显示这个element是li

list属性描述li的基本样式：允许指定marker的type（image， glyph， number），以及指定marker相对于主块的位置(outside, inside), 不允许指定marker的样式和相对位置。

background，只作用与主块，marker的是透明的。

- `list-style-position`
	- outside: marker-box在主块之外：
		- 没有定义与float相邻的定位位置
		- 没有定义marker box具体的位置，和paint order
		- li dir：ltr，marker在左边；反之，右边
		- marker box固定与主块的border，不随着主块内容滚动而滚动
		- 若主块的overflow非visible，有可能导致marker无法显示（要改）
		- marker box的大小和内容有可能影响主块或其第一个line box的高度。某些情况下，可能引起新行的创建
	- inside: 作为主块的第一个line box，位于`::before`之前


## 确认方案
1. 生成marker的时候，style是否确定？`inside->inline marker`; `outside->absolute marker`;
2. absolute marker 需要重载container为li(MPRE), dir?一个static的containing block加上abs的孩子，会有问题嘛？有问题，根本无效。。所以，如果不自己创建container的话，只能让marker向上找container，并根据这个container的位置定位marker，相信我，很麻烦！不要问我怎么知道，我当然是写demo了，并没有把定位定好...如果自己创建container，那么是不是abs就无所谓了，可以是relative。哈哈，不要管container，因为如果没设置位置，直接用static的位置，需要的layoutlistmarker::layout里面调用一下updatelogicalwidth等即可。
3. 添加marker到layout tree：  inline marker添加到第一个line box的父亲节点中; absolute marker添加到li中
4. after layout， absolute marker更新垂直方向的位置；inline marker更新水平方向的位置？不要
5. 切换position时，需要删除marker，重新生成（或者更新style）
6. painter需要修改嘛？
7. 跟line box相关的函数要改嘛？
8. line-height: 内容有line-height的情况， li自己有line-height的情况
9. image marker?
10. LayoutListItem要改，box, block?



## 各种object

- LayoutObject
	- LayoutText
	- *LayoutBoxModel*
		- *LayoutInline*
		- *LayoutBox*
			- LayoutReplace
			- *LayoutListMarker*
			- LayoutBlock
				- *LayoutBlockFlow*
				- flex
				- grid
				- table
				- scrollBar
