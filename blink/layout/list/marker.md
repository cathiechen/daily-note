# 重构marker position的实现


## W3C中关于listMarker的规定

- 现有规定：
- 后续可能添加的规定：


## 现有方案

- `ListMarker`都是inline的replace. 不是个block，是个box，没有孩子，自己直接是内容
- `LayoutListItem::SubtreeDidChange()`时，调用`LayoutListItem::UpdateMarkerLocation()`;
  - 确定`line_box_parent`
    - 第一个line_box的父亲节点
    - `<li>`
  - remove marker from 之前的节点
  - `line_box_parent->AddChild(marker_, FirstNonMarkerChild(line_box_parent))`
  - `marker_->UpdateMarginsAndContent();`更新marker的content和margin(偏移设置在margin里面), 设置margin导致，box向左（ltr）偏移，marker偏移量是preferwidth + padding.
    - `LayoutListMarker`的layout，没有走box的的layout，直接自己设置宽高和margin
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
  - 创建一个inline的marker
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

## list marker container的问题

1. 先来考虑是否必要吧。
一开始不想要container，直接让marker用自己的static position。但后面遇到text-align: right的问题，即：static position是不确定的，会被css改变。而且不确定的container让人很不安。

2. 创建container的问题
首先inside marker是不需要的，只有outside marker需要container。在outside和inside切换时，尤其要小心。所以，具体方法就是，把container保存的list item中：
	- 创建： 在list item的styledidchange中，确定是outside marker，才创建
	- destroy：在list item销毁之前销毁
	- 当marker是新建的outside时，把marker加到container，并把container加入list item
	- 当marker是新建的inside时，不会创建container，直接加入list item
	- 当marker从outside变成inside时， 把container从layout tree上remove掉，然后marker直接加入list item
	- 当marker从inside变成outside时， 如果没有container创建container，然后marker加入container，然后container加入list item

3. container的css property
	- display: block
	- position: relative. 因为它是一个absolute的container。
	- height: 0px. 为了不影响list item的内容

4. 如果有了container，outside marker是否没必要absolute
不行啊，如果不是absolute的，那么marker将可以被float入侵，导致偏移。

5. containingblock不能是anonymous的问题
containingblock不能是anonymous的理由是： anonymous可能随时被合并。但marker container不存在这样的问题啊，它和marker都随着list item一起destroy。所以不存在marker还在，container就被删除的风险。

以上！


## 修改

1. marker container is anonymous,对containingBlock的影响
2. marker container相关修改
	- 创建
	- 销毁
	- 如何添加到layout tree
3. marker的css属性设置（absolute， static）
4. marker的layout处理
5. list item computeoverflow时，需要处理marker的位置。因为marker的layout跑到了content前面，所有没法在marker layout时，对齐其和content的baseline！！！
6. 好多crash:( 而且面对float入侵，和以前的表现不同。 啊！要疯了！！


## 对文字排版有影响的CSS属性
- direction: ltr|rtl， rtl会导致有些文字从右到左，其中标点符号一定会在最左。(只有direction会影响marker是出现在哪边)
- text-align: right，文字的顺序不会变化，但还会整体右移。
- vertical-align: baseline|top|bottom等，是当前元素和父元素的对齐方式
- writing-mode: vertical-lr，控制文字是否垂直排版，以及文字的排版方向


## 修改2

1. inline marker parent, 确保marker的static位置和之前的一样
2. 与container相似，在list item中： 创建，销毁，添加到layout tree（marker加到parent，parent加到inline里面）
3. marker属性设置：absolute
4. marker layout时处理，计算static位置,计算偏移位置（abs的layout在普通元素之后）

存在的问题：
1. crash
加container的方案都有这个问题，原因是释放了两遍：1. parent node释放 2.li 释放。解决方法：parent时不让释放

2. 显示不了marker
原因是container的width以及overflow rect算出来的宽度都是0. width是0正常。 overflow rect不该是0, 移植positionmarkerposition到addoverflowfromchild后，可以显示.从这里，也可以看出overflow rect的作用。

3. 如果first line box的位置发生变化，container的位置将不会被更新，好蛋疼！shit，这个可以否定掉所有absolute的方案...

## 目前试过的方案
方案的主要区别在于marker添加的位置

- relative container
	- 建一个relative的container，把marker设置成abs并加到container， container加到li下面。
	- 然后再水平对齐，从左边float等情况的处理
	- 垂直对齐，找到firstlinebox，并对齐
- inline container
	- 创建一个inline的container，把marker设置成abs并加到container，container加到first line box的前面，减少垂直对齐的工作量
	- 水平对齐：从右边float等情况处理，现成代码可以参考
	- 垂直对齐：从firstlinebox的底部开始对齐
- 直接插入abs的marker
	- 直接插入abs的marker到firstlinebox的前面
	- 水平对齐：从右边float等情况处理，现成代码可以参考
	- 垂直对齐：从parent的(0，0)位置开始对齐，lineheight等情况需要处理
- 创建abs container + static marker（感觉这个方法最好）
	- abs container 加到firstlinebox前面，copy li的positionlistmarker到container的addoverflowfromchild，可以正常显示。container的style继承parent还是li的问题待解决。（几乎快要成功了，但是firstlinebox位置变化的问题无法解决）
	- 水平对齐，用现成的
	- 原来不用container是有道理的，container css的继承问题很混乱。

啊啊啊，心好累，单纯用css无法解决li的问题！

尝试让marker作为一个layout被当成static，paint被当成abs的obj的方法。

其实，最开始的那个方案是应该是可以的：
- marker直接加到li，如果创建了anonymous block，设置其高度为0.（这样可以不影响content的垂直位置）
- 对齐marker的linebox和content的第一个linebox的baseline(可以复用positionlistmarker的代码)
- 这样位置变化的问题，就可以解决了。因为对齐用的是之前的方案，那么位置应该也可以保证一致。
- vertical-align的问题，baseline， top，bottom

添加invisible marker：
- 创建、删除、remove，更新内容等，和`marker_`差不多
- 设置invisible marker的visibility:hidden

markers add的位置：
- invisible marker加到原来的位置,invisible marker要在marker之前添加，因为marker加完后有可能影响layout tree导致`line_box_parent`被删除。
- marker尽量加成li的第一个孩子

list的第一个孩子是匿名块的问题：
- 匿名块有自己的内容：visible marker会被加到`<ab>`里面，所以此时marker不该设置自己的parent为（0,0），只有自己是anonymousblock唯一孩子时，才可以把ab设置成（0,0）
```
<li>
  <ab>
    <marker></marker>
    <span>anonymous block content<span>
  </ab>
  <div>another block</div>
</li>
```
- 匿名块没有自己的内容，若把marker加到该ab里面，将会导致新生一行，影响排版。所以改成把marker加到firstlinebox的li的下一级且匿名的祖先节点中。
```
<li>
  <ab>
    <span>
  </ab>
  <ab>
    <ab><marker></marker></ab>
    <div>another block</div>
  </ab>
  <ab>
    </span>
  </ab>
</li>
```

把marker移动到invisible marker的位置。
- 若没有invisible marker，走原来调整位置且计算overflow的逻辑
- 存在invisible marker，先计算invisible marker的位置，移动marker的位置到invisible marker，计算marker的overflow，若不计算overflow将显示不了。

添加到list item下面的marker会导致margin collapse的中断
- 把marker的anonymous parent当成position之类的节点，不影响margin collapse的传递
- layout 第一个孩子之后`margin_info.SetAtBeforeSideOfBlock(false);`，但marker不应该影响下一个孩子的margin，所以在这里skip掉这个设置。
