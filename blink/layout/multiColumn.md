# Multi-column layout

又翻译了一篇[文章](https://www.chromium.org/developers/design-documents/multi-column-layout)，棒棒哒～

multi-column不算新了，但为啥现在才看呢？

这是一种类似print的排版（print排版是怎样的？我也不知道...）主要是对content进行分列排版。主要流程：先用flow thread对content进行排版。然后在分flow thread。 只看文档理解不够深刻，后续还是要调个bug看看。

## CSS

- `columns: 3;`代表内容要被平分成3列。还可以定义列与列之间的宽度。粗略算起来，一个column的宽度就是container_width / column_count.
- widows/orphans：这两个理解花了好长时间
	- `widows: 3`代表当前column没办法排下这个段落时，在新的column头部，这个段落至少3行
	- `orphans: 3`代表在当前column的尾部，开始的新段落至少3行才能换column。

## 概括说明
### Flow Thread: 

- 浏览器默认创建的匿名块
- 用column的宽度 layout所有内容
- 只在paint 或 hittest时需要break this thread
- container所有孩子dom节点都flow thread的孩子
- flow thread不占空间，不能被paint或hittest。所以，需要创建一个兄弟节点(LayoutMultiColumnSet)占空间

### LayoutMultiColumnSet

- 用于占用空间
- 负责坐标转换(flow thread 和 visual coordinate)
- 代表一个或多个column
- 没有孩子节点
- 包含多个MultiColumnFragmentainerGroup

## 定义

如何区分flowthread和visual coordinate的坐标

1. flow thread coordinate： 只用column的宽度排版所有孩子，它的坐标是独立的。
2. visual coordinate: paint和hittest的坐标，即普通的坐标。
3. Flow thread portion rectangle: 每个column都有一个，定义该column对应的flow thread部分。以rectangle的方式存储，因为每个column的内容高度可能不同。

## [example](https://www.chromium.org/developers/design-documents/multi-column-layout)

一定要看。很帮助理解

## pagination struts: 如何分页

如果内容不够一行（或widows or orphans的情况），column的底部会有一些空白，这些空白由pagination struts填充

## column balance： 基于content计算column的高度

- 什么时候需要balance：1. 高度没有设置；2. column-fill: 'balance' 3. column-span: all;
- 算法：
	- layout flow thread，不管break的情况，高度H
	- guess height： H / column number
	- 考虑所有的分页情况，重新layout flow thread
	- 如果太多column了，给高度加上`Minimum space shortage`, 重新layout，直到fit
- `Minimum space shortage`： 最小的防止过多break的高度

## nested fragmentation context:

嵌套的情况会发生，如果multi-column遇到分页，先把第一页填满，然后再开始新的一页

## 总结

这个算法挺有趣的。若是我来实现，我可能还会用table类似的算法，先分column，然后在对其进行排版。这个是正向的思维。但该算法思维上有点逆向，先排好，然后在分割成column。不知道实际过程中，是否会遇到问题，反正，我觉得挺好。
