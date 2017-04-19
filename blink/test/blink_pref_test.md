# chrome的blink_pref性能测试

[chromium性能测试报告网站](https://chromeperf.appspot.com/)

所提的patch，经历了好多波折，又在一个叫blink_pref的性能测试中倒下了，具体见[这里](https://bugs.chromium.org/p/chromium/issues/detail?id=675534&can=2&start=0&num=100&q=&colspec=ID%20Pri%20M%20Stars%20ReleaseBlock%20Component%20Status%20Owner%20Summary%20OS%20Modified&groupby=&sort=)。

在跑一个叫flexbox-lots-of-data.html的测试用例时性能降低了20%~30%.

仔细研究了一下这个用例：

- 很长的flex
- 来回变动body的宽度,并用一种方法让body以下节点reattach，重复测试20次
- 最后结果输出到页面中

感觉整个流程真的很妙，这样就只测试css变化引起的layout时间耗时。给力！


## 如何触发排版

    document.body.style.width = ++index % 2 ? "99%" : "98%";
    document.body.offsetHeight;
以上两句js触发的.

首先，改变body的宽度，使得body重新计算style，并告诉父亲节点需要重新layout孩子节点。

document.body.offsetHeight会触发layout。

layout过程中，layout到body的时候，由于body本身的宽度变了，所以relayoutChildren=true.

	inline bool LayoutBlockFlow::layoutBlockFlow(bool relayoutChildren, LayoutUnit &pageLogicalHeight, SubtreeLayoutScope& layoutScope)
	{
	    LayoutUnit oldLeft = logicalLeft();
	    bool logicalWidthChanged = updateLogicalWidthAndColumnWidth();
	    relayoutChildren |= logicalWidthChanged;
		......
	}

在layout body的孩子节点之前，有一个这样的操作： updateBlockChildDirtyBitsBeforeLayout(relayoutChildren, *child)；作用是更新孩子节点的Bits，注意看relayoutChildren被传下来了。

	void LayoutBlock::updateBlockChildDirtyBitsBeforeLayout(bool relayoutChildren, LayoutBox& child)
	{
	    if (child.isOutOfFlowPositioned()) {
	        // It's rather useless to mark out-of-flow children at this point. We may not be their
	        // containing block (and if we are, it's just pure luck), so this would be the wrong place
	        // for it. Furthermore, it would cause trouble for out-of-flow descendants of column
	        // spanners, if the containing block is outside the spanner but inside the multicol container.
	        return;
	    }
	    // FIXME: Technically percentage height objects only need a relayout if their percentage isn't going to be turned into
	    // an auto value. Add a method to determine this, so that we can avoid the relayout.
	    bool hasRelativeLogicalHeight = child.hasRelativeLogicalHeight()
	        || (child.isAnonymous() && this->hasRelativeLogicalHeight())
	        || child.stretchesToViewport();
	    if (relayoutChildren || (hasRelativeLogicalHeight && !isLayoutView()))
	        child.setChildNeedsLayout(MarkOnlyThis);
		......
	}
这样child.setChildNeedsLayout就会被layout了。
	
		minichrome.exe!blink::LayoutObject::setNormalChildNeedsLayout(bool b) 行 1424	C++
		minichrome.exe!blink::LayoutObject::setChildNeedsLayout(blink::MarkingBehavior markParents, blink::SubtreeLayoutScope * layouter) 行 1547	C++
	>	minichrome.exe!blink::LayoutBlock::updateBlockChildDirtyBitsBeforeLayout(bool relayoutChildren, blink::LayoutBox & child) 行 1020	C++
		minichrome.exe!blink::LayoutBlockFlow::updateBlockChildDirtyBitsBeforeLayout(bool relayoutChildren, blink::LayoutBox & child) 行 1907	C++
		minichrome.exe!blink::LayoutBlockFlow::layoutBlockChildren(bool relayoutChildren, blink::SubtreeLayoutScope & layoutScope, blink::LayoutUnit beforeEdge, blink::LayoutUnit afterEdge) 行 979	C++
		minichrome.exe!blink::LayoutBlockFlow::layoutBlockFlow(bool relayoutChildren, blink::LayoutUnit & pageLogicalHeight, blink::SubtreeLayoutScope & layoutScope) 行 377	C++
		minichrome.exe!blink::LayoutBlockFlow::layoutBlock(bool relayoutChildren) 行 297	C++
		minichrome.exe!blink::LayoutBlock::layout() 行 858	C++
		minichrome.exe!blink::LayoutBlockFlow::layoutBlockChild(blink::LayoutBox & child, blink::MarginInfo & marginInfo, blink::LayoutUnit & previousFloatLogicalBottom) 行 547	C++
		minichrome.exe!blink::LayoutBlockFlow::layoutBlockChildren(bool relayoutChildren, blink::SubtreeLayoutScope & layoutScope, blink::LayoutUnit beforeEdge, blink::LayoutUnit afterEdge) 行 1004	C++
		minichrome.exe!blink::LayoutBlockFlow::layoutBlockFlow(bool relayoutChildren, blink::LayoutUnit & pageLogicalHeight, blink::SubtreeLayoutScope & layoutScope) 行 377	C++
		minichrome.exe!blink::LayoutBlockFlow::layoutBlock(bool relayoutChildren) 行 297	C++
		minichrome.exe!blink::LayoutBlock::layout() 行 858	C++
		minichrome.exe!blink::LayoutBlockFlow::layoutBlockChild(blink::LayoutBox & child, blink::MarginInfo & marginInfo, blink::LayoutUnit & previousFloatLogicalBottom) 行 547	C++
		minichrome.exe!blink::LayoutBlockFlow::layoutBlockChildren(bool relayoutChildren, blink::SubtreeLayoutScope & layoutScope, blink::LayoutUnit beforeEdge, blink::LayoutUnit afterEdge) 行 1004	C++
		minichrome.exe!blink::LayoutBlockFlow::layoutBlockFlow(bool relayoutChildren, blink::LayoutUnit & pageLogicalHeight, blink::SubtreeLayoutScope & layoutScope) 行 377	C++
		minichrome.exe!blink::LayoutBlockFlow::layoutBlock(bool relayoutChildren) 行 297	C++
		minichrome.exe!blink::LayoutBlock::layout() 行 858	C++
		minichrome.exe!blink::LayoutView::layoutContent() 行 153	C++
		minichrome.exe!blink::LayoutView::layout() 行 242	C++
		minichrome.exe!blink::layoutFromRootObject(blink::LayoutObject & root) 行 802	C++
		minichrome.exe!blink::FrameView::performLayout(bool inSubtreeLayout) 行 864	C++
		minichrome.exe!blink::FrameView::layout() 行 1032	C++
		minichrome.exe!blink::Document::updateLayout() 行 1907	C++
		minichrome.exe!blink::Document::updateLayoutIgnorePendingStylesheets(blink::Document::RunPostLayoutTasks runPostLayoutTasks) 行 1977	C++
		minichrome.exe!blink::Element::offsetHeight() 行 607	C++
		minichrome.exe!blink::HTMLElementV8Internal::offsetHeightAttributeGetter(const v8::FunctionCallbackInfo<v8::Value> & info) 行 534	C++

over!
