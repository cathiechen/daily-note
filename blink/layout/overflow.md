#overflow

传说中，有一条overflow的线，在layout中存在了很久，但一直都没被我注意到。

overflow存在于LayoutBox，代表溢出，所谓溢出，就是有超出本身box模型。有两种，`m_layoutOverflowRect`; visualOverflowRect: `m_selfVisualOverflowRect`, `m_contentVisualOverflowRect`.这次看的主要是layoutoverflowRect。 block和line甚至是Glyph，都有overflow。 但Glyph的overflow好像不是基于这个模型的。见LayoutText.md

	class LayoutBox {
		...
		// Our overflow information.
	  	std::unique_ptr<BoxOverflowModel> m_overflow;
		...
	}

##overflow的形成

- when: 排版的后期，即：确定x，y，w，h，排完孩子以及positon孩子之后，调用computeOverflow, 具体见：`LayoutBlockFlow::layoutBlock`. inline也是一样，见：`LayoutBlockFlow::createLineBoxesFromeBidiRuns`。
- computeOverflow中决定了overflow都有哪些方面组成：
	- LayoutBlockFlow::computeOverflow
		- addoverflowfromchild
		- addOverflowFromPositionedObjects
		- hasOverflowClip ? addLayoutOverflow(rectToApply)
		- addVisualEffectOverflow()
		- addVisualOverflowFromTheme();
		- addOverflowFromFloats();
	- InlineFlowBox::computeOverflow
		- logicalFrameRectIncludingLineHeight(lineTop, lineBottom)
		- add overflow from child boxes
			- textBoxOverflow(text)
			- logicalLayoutOverflowRect(lineTop, lineBottom)(inline)
			- addReplacedChildOverflow(replace)

##overflow的使用