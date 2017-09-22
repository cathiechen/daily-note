# block layout

本文档记录block layout相关的笔记。

## margin collapse

margin的塌陷，简言之：
- 嵌套的box，若box中间没有border、padding等阻挡，margin将取最大的那个，而不是相加。
- 上下相邻box，上下的margin将会合并，取大的那个。
[详见](https://css-tricks.com/what-you-should-know-about-collapsing-margins/)

如何实现的？
主要逻辑在`blink::LayoutBlockFlow::layoutBlockChild`，父节点负责设置child的logicalTop。

- 在layoutblockchild前期，利用`blink::LayoutBlockFlow::estimateLogicalTopPosition`计算出最大的margintop，设置logicaltop
	- `LayoutBlockFlow::estimateLogicalTopPosition`，会逐级往下找到第一个符合要求的孩子，计算其margininfo，找到最大的margin。维护margininfo，这样这些处理过的孩子就不需要在处理的。遇到不符合要求的孩子，跳出这个stack。
	- layoutchild
	- layoutchild之后，会得到一个确定的logicaltop，并可以通过collapsemargins计算出下一个兄弟节点的margintop，通过修改当前节点的margininfo来维护。
	- margininfo正确设置后，对下一个孩子节点重复上述过程。
	- 这个算法把父亲节点和兄弟节点的margin collapse用同一种逻辑处理，挺妙的。
	- 还有margininfo的维护，只有开始一个新的stack才需要逐级计算margin。
	- 这个对排版来说其实是很重要的一个功能，但平时很少出错，所以基本没有看过该部分代码。。
```
>
minichrome.exe!blink::LayoutBlockFlow::marginBeforeEstimateForChild(blink::LayoutBox & child, blink::LayoutUnit & positiveMarginBefore, blink::LayoutUnit & negativeMarginBefore, bool & discardMarginBefore) 行 1396
C++
 minichrome.exe!blink::LayoutBlockFlow::marginBeforeEstimateForChild(blink::LayoutBox & child, blink::LayoutUnit & positiveMarginBefore, blink::LayoutUnit & negativeMarginBefore, bool & discardMarginBefore) 行 1429
C++
 minichrome.exe!blink::LayoutBlockFlow::marginBeforeEstimateForChild(blink::LayoutBox & child, blink::LayoutUnit & positiveMarginBefore, blink::LayoutUnit & negativeMarginBefore, bool & discardMarginBefore) 行 1429
C++
 minichrome.exe!blink::LayoutBlockFlow::marginBeforeEstimateForChild(blink::LayoutBox & child, blink::LayoutUnit & positiveMarginBefore, blink::LayoutUnit & negativeMarginBefore, bool & discardMarginBefore) 行 1429
C++
 minichrome.exe!blink::LayoutBlockFlow::estimateLogicalTopPosition(blink::LayoutBox & child, const blink::MarginInfo & marginInfo, blink::LayoutUnit & estimateWithoutPagination) 行 1443
C++
 minichrome.exe!blink::LayoutBlockFlow::layoutBlockChild(blink::LayoutBox & child, blink::MarginInfo & marginInfo, blink::LayoutUnit & previousFloatLogicalBottom) 行 510
C++
 minichrome.exe!blink::LayoutBlockFlow::layoutBlockChildren(bool relayoutChildren, blink::SubtreeLayoutScope & layoutScope, blink::LayoutUnit beforeEdge, blink::LayoutUnit afterEdge) 行 1004
C++
 minichrome.exe!blink::LayoutBlockFlow::layoutBlockFlow(bool relayoutChildren, blink::LayoutUnit & pageLogicalHeight, blink::SubtreeLayoutScope & layoutScope) 行 377
C++
```
