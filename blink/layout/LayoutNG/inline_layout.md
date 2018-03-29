

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
