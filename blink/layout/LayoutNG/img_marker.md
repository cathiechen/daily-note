# image marker

LayoutNG中image的实现逻辑。cr还没错过，还没上传，目前的逻辑是这样的。

## layout tree结构

与老的的marker不同，ng中marker是个inline block或inline。

ng中marker的内容自己拥有一个layout节点，老的方案中，marker就是内容节点，可以是text也可以是image。

旧实现的layout tree：

```
            LayoutBlockFlow 0x1a5864a18700	UL
              LayoutListItem 0x1a5864a083d0	LI
*               LayoutListMarker (anonymous) 0x1a5864a18828
                LayoutText 0x1a5864a24010	#text "LI text is here too"
```

NG的layout tree：

```
            LayoutNGBlockFlow 0x33ff860187c0	UL
              LayoutNGListItem 0x33ff86028170	LI
                LayoutInline (anonymous) 0x33ff86044010
*                 LayoutImage (anonymous) 0x33ff86050010
                LayoutText 0x33ff860341b0	#text "LI text is here too"
```


## 需要特别解决的问题

### text marker和image marker之间的切换

以前只要改变marker的节点就好了，现在要改变layout tree。

有两种切换：
1. style change的切换，要在styledidchange时更新layout tree

2. image error被动切换成text。以前如果error，只要重新update layout即可，但现在要update layout tree。
方案是：新建一个markeriamge类，继承与layoutimage，若image error occur，发送一个subtreechange事件。li监听subtreechanged事件，在subtreedidchange的回调中，更新layout tree。

### svg image的size计算问题

svg 图片的内在size需要传一个default size，不然就只能取到0 * 0。
解决方案：override layoutimage的GetNestedIntrinsicSizingInfo，如果是svg，切没有size，通过传入default size来获取size。
`ToSVGImage(image)->ConcreteObjectSize(default_size))`

### vertical mode下的一个像素的差异

```
#0  0x00007fffea355f6d in blink::NGInlineLayoutStateStack::AddBoxFragmentPlaceholder(blink::NGInlineBoxState*, blink::NGLineBoxFragmentBuilder::ChildList*, blink::FontBaseline) (this=0x1f1a3b30b3a0, box=0x1f1a3b30b428, line_box=0x7fffba31e238, baseline_type=blink::kIdeographicBaseline) at ../../third_party/WebKit/Source/core/layout/ng/inline/ng_inline_box_state.cc:230
#1  0x00007fffea355c91 in blink::NGInlineLayoutStateStack::EndBoxState(blink::NGInlineBoxState*, blink::NGLineBoxFragmentBuilder::ChildList*, blink::FontBaseline) (this=0x1f1a3b30b3a0, box=0x1f1a3b30b428, line_box=0x7fffba31e238, baseline_type=blink::kIdeographicBaseline) at ../../third_party/WebKit/Source/core/layout/ng/inline/ng_inline_box_state.cc:180
#2  0x00007fffea355c2d in blink::NGInlineLayoutStateStack::OnCloseTag(blink::NGLineBoxFragmentBuilder::ChildList*, blink::NGInlineBoxState*, blink::FontBaseline) (this=0x1f1a3b30b3a0, line_box=0x7fffba31e238, box=0x1f1a3b30b428, baseline_type=blink::kIdeographicBaseline) at ../../third_party/WebKit/Source/core/layout/ng/inline/ng_inline_box_state.cc:158
#3  0x00007fffea36e824 in blink::NGInlineLayoutAlgorithm::CreateLine(blink::NGLineInfo*, blink::NGExclusionSpace*) (this=0x7fffba31e130, line_info=0x7fffba31cf70, exclusion_space=0x1f1a3b350320)
    at ../../third_party/WebKit/Source/core/layout/ng/inline/ng_inline_layout_algorithm.cc:159
#4  0x00007fffea3729a1 in blink::NGInlineLayoutAlgorithm::Layout() (this=0x7fffba31e130) at ../../third_party/WebKit/Source/core/layout/ng/inline/ng_inline_layout_algorithm.cc:568
#5  0x00007fffea37ceb3 in blink::NGInlineNode::Layout(blink::NGConstraintSpace const&, blink::NGBreakToken*) (this=0x7fffba31fc98, constraint_space=..., break_token=0x0)
    at ../../third_party/WebKit/Source/core/layout/ng/inline/ng_inline_node.cc:436
#6  0x00007fffea3cfb39 in blink::NGLayoutInputNode::Layout(blink::NGConstraintSpace const&, blink::NGBreakToken*) (this=0x7fffba31fc98, space=..., break_token=0x0)
    at ../../third_party/WebKit/Source/core/layout/ng/ng_layout_input_node.cc:126
#7  0x00007fffea3ad8e1 in blink::NGBlockLayoutAlgorithm::HandleInflow(blink::NGLayoutInputNode, blink::NGBreakToken*, blink::NGPreviousInflowPosition*, scoped_refptr<blink::NGBreakToken>*) (this=0x7fffba321330, child=..., child_break_token=0x0, previous_inflow_position=0x7fffba320058, previous_inline_break_token=0x7fffba320050)
    at ../../third_party/WebKit/Source/core/layout/ng/ng_block_layout_algorithm.cc:909

```



老的inlinelayout：

```
#0  0x00007fffea31de24 in blink::InlineFlowBox::PlaceBoxesInBlockDirection(blink::LayoutUnit, blink::LayoutUnit, blink::LayoutUnit, bool, blink::LayoutUnit&, blink::LayoutUnit&, blink::LayoutUnit&, bool&, blink::LayoutUnit&, blink::LayoutUnit&, bool&, bool&, blink::FontBaseline) (this=0x2b24ee23c0d8, top=..., max_height=..., max_ascent=..., no_quirks_mode=false, line_top=..., line_bottom=..., selection_bottom=..., set_line_top=@0x7fffba32691f: false, line_top_including_margins=..., line_bottom_including_margins=..., has_annotations_before=@0x7fffba32691e: false, has_annotations_after=@0x7fffba32691d: false, baseline_type=blink::kAlphabeticBaseline) at ../../third_party/WebKit/Source/core/layout/line/InlineFlowBox.cpp:774
#1  0x00007fffea33e393 in blink::RootInlineBox::AlignBoxesInBlockDirection(blink::LayoutUnit, WTF::HashMap<blink::InlineTextBox const*, std::__1::pair<WTF::Vector<blink::SimpleFontData const*, 0ul, WTF::PartitionAllocator>, blink::GlyphOverflow>, WTF::PtrHash<blink::InlineTextBox const>, WTF::HashTraits<blink::InlineTextBox const*>, WTF::HashTraits<std::__1::pair<WTF::Vector<blink::SimpleFontData const*, 0ul, WTF::PartitionAllocator>, blink::GlyphOverflow> >, WTF::PartitionAllocator>&, blink::VerticalPositionCache&) (this=0x2b24ee23c0d8, height_of_block=..., text_box_data_map=..., vertical_position_cache=...) at ../../third_party/WebKit/Source/core/layout/line/RootInlineBox.cpp:283
#2  0x00007fffea19ed82 in blink::LayoutBlockFlow::ComputeBlockDirectionPositionsForLine(blink::RootInlineBox*, blink::BidiRun*, WTF::HashMap<blink::InlineTextBox const*, std::__1::pair<WTF::Vector<blink::SimpleFontData const*, 0ul, WTF::PartitionAllocator>, blink::GlyphOverflow>, WTF::PtrHash<blink::InlineTextBox const>, WTF::HashTraits<blink::InlineTextBox const*>, WTF::HashTraits<std::__1::pair<WTF::Vector<blink::SimpleFontData const*, 0ul, WTF::PartitionAllocator>, blink::GlyphOverflow> >, WTF::PartitionAllocator>&, blink::VerticalPositionCache&) (this=0x2b24ee2083d0, line_box=0x2b24ee23c0d8, first_run=0xa7ce325c7a8, text_box_data_map=..., vertical_position_cache=...) at ../../third_party/WebKit/Source/core/layout/LayoutBlockFlowLine.cpp:886
#3  0x00007fffea19f349 in blink::LayoutBlockFlow::CreateLineBoxesFromBidiRuns(unsigned int, blink::BidiRunList<blink::BidiRun>&, blink::InlineIterator const&, blink::LineInfo&, blink::VerticalPositionCache&, blink::BidiRun*, WTF::Vector<blink::WordMeasurement, 64ul, WTF::PartitionAllocator> const&) (this=0x2b24ee2083d0, bidi_level=0, bidi_runs=..., end=..., line_info=..., vertical_position_cache=..., trailing_space_run=0x0, word_measurements=...) at ../../third_party/WebKit/Source/core/layout/LayoutBlockFlowLine.cpp:953
#4  0x00007fffea1a1325 in blink::LayoutBlockFlow::LayoutRunsAndFloatsInRange(blink::LineLayoutState&, blink::BidiResolver<blink::InlineIterator, blink::BidiRun, blink::BidiIsolatedRun>&, blink::InlineIterator const&, blink::BidiStatus const&) (this=0x2b24ee2083d0, layout_state=..., resolver=..., clean_line_start=..., clean_line_bidi_status=...)
    at ../../third_party/WebKit/Source/core/layout/LayoutBlockFlowLine.cpp:1200
#5  0x00007fffea19f685 in blink::LayoutBlockFlow::LayoutRunsAndFloats(blink::LineLayoutState&) (this=0x2b24ee2083d0, layout_state=...)
    at ../../third_party/WebKit/Source/core/layout/LayoutBlockFlowLine.cpp:1008
#6  0x00007fffea1a597e in blink::LayoutBlockFlow::LayoutInlineChildren(bool, blink::LayoutUnit) (this=0x2b24ee2083d0, relayout_children=true, after_edge=...)
    at ../../third_party/WebKit/Source/core/layout/LayoutBlockFlowLine.cpp:2000
#7  0x00007fffea17b194 in blink::LayoutBlockFlow::LayoutChildren(bool, blink::SubtreeLayoutScope&) (this=0x2b24ee2083d0, relayout_children=true, layout_scope=...)
    at ../../third_party/WebKit/Source/core/layout/LayoutBlockFlow.cpp:588

```


