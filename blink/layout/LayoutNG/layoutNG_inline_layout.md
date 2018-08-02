# LayoutNG Inline Layout

翻译[这篇](https://chromium.googlesource.com/chromium/src/+/master/third_party/WebKit/Source/core/layout/ng/inline/README.md)

Following DOM tree is transformed to fragment tree as in the following.

|||---|||
### DOM ###

```html
<div>
  <span>
    Hello
  </span>
</div>
```

### NGLayoutInputNode ###

* NGBlockNode
  - NGInlineNode
    - NGInlineItem (open tag, span)
    - NGInlineItem (text, "Hello")
    - NGInlineItem (close tag, span)

### Fragment tree ###

* NGPhysicalBoxFragment
  - NGPhysicalBoxFragment (anonymous wrapper)
    - NGPhysicalLineBoxFragment
      - NGPhysicalBoxFragment (span, may be omitted)
        - NGPhysicalTextFragment ("Hello")
|||---|||


## Inline Layout Phases
inline layout分一下步骤完成：

- pre-layout：把layout object转换成串联的string和NGInlineItem list
- line breaking:将上面产生的结果分成不同的行，每一行产生一个NGInlineItemResult list。
- line box construction:生成fragment tree


### 1. `Pre-layout`

inline layout有一个pre layout的过程，为inline layout准备需要的数据结构。pre layout通过`NGInlineNode::PrepareLayout()`调用。分成以下步骤
  1.1 `CollectInlines`：深度遍历container，收集non-atomic inline节点和text节点。Atomic inlines 由一个unicode object来表示，若不能用unicode object表示，则跳过。每个non-atomic inline 和 TextNodes保存到NGInlineItemsBuilder 中。在此过程中，同时处理好white-space。css的text-transform目前在layout tree中实现，后续想移动到这里。（cc：得到NGInlineItem vector, 主要由NGInlineItemsBuilder）
  1.2 `SegmentText`：处理bidi的分段和解析。
  1.3 `ShapeText`：用harfbuzz获取resolved bidi run的形状(cc：`item.shape_result_`就是在这里得到的)

### 2. line break

`NGLineBreaker`处理NGInlineItem：计算宽度，分行，然后每行产生一个NGInlineItemResult list。NGInlineItemResult中保存一些box construction需要的信息，比如： inline size 和 ShapeResult, Bidirectional text可能会导致后续需要重新计算。（cc：LineData提供line相关的信息）
line break的步骤：
  2.1 Measures each item.
  2.2 把text NGInlineItem 放入multiple NGInlineItemResult中。主要实现逻辑在：ShapingLineBreaker.
  2.3 计算borders/margins/paddings的inline size。inline non-replaced elements此时忽略borders/margins/paddings的block size计算。因为只有inlne size会影响layout和分行。see [CSS Calculating widths and margins](https://drafts.csswg.org/css2/visudet.html#Computing_widths_and_margins)
  2.4 决定哪些item可以放进此行。
  2.5 决定items之间的break opportunity。If an item overflows, and there were no break opportunity before it, the item before must also overflow.（你说，这个怎么翻？！）

### 3. Line Box Construction

`NGInlineLayoutAlgorithm::CreateLine()`以NGInlineItemResult list为输入，给每一行输出一个NGPhysicalLineBoxFragment。
lines被包含在一个匿名的NGPhysicalBoxFragment中，这样每个 NGInlineNode都有一个对应的fragment。
Line Box Construction的步骤：
  3.1 Bidirectional reordering：根据[这个协议](http://www.unicode.org/reports/tr9/#Reordering_Resolved_Levels)重新排序NGInlineItemResult。从此以后，NGInlineItemResult 就是以visual order排序了：从左到右。但此时block方向还是logical的。
  3.2 创建NGPhysicalFragment for each NGInlineItemResult in visual order，然后把他们放入NGPhysicalLineBoxFragment中。
    3.2.1 text item产生NGPhysicalTextFragment
    3.2.2 遇到open-tag item入栈一个 NGInlineBoxState，遇到close-tag item 出栈。为要求自己size的inline box 或要求祖先size的inline box计算size。 See Inline Box Tree below.
inline size：在inlne breaker时已经确定，但有可能在Bidirectional reordering中被改变。
block size：后续再算。
  3.3 创建line box后的后续处理，包括：
    3.3.1 处理Inline Box Tree中挂起的操作
    3.3.2 基于line box的height，移动baseline到正确的位置
    3.3.3 Applies the CSS text-align property.

#### Inline Box Tree

扁平的数据结构适用于大多数情况。但有时候要求line box tree。在LayoutNG中，由NGInlineBoxState堆栈代表 box tree 结构。

这个堆栈包括：
1. Caches common values for an inline box. 比如：primary font metrics不会在inline box中改变。
2. 计算inline box的高度。inline non-replace的高度由content area决定，当css没有定义，如何计算。
3. 必要时创建NGPhysicalBoxFragment。没有明确说明每个inline box都必须是个box，但有border，background或scroll bar时，会需要一个box
4. 根据vertical-align改变baseline。有些value不能马上计算出值来。有些value需要box或parent box的size。有些则需要root inline box的size。全部取决于设置的value。。
在OnEndPlaceItems()中把result转换成fragment。

#### Box Fragments in Line Box Fragments

不是所有的inline-level box都需要创建box fragment。如：有border的span，可以通过`NGInlineBoxState::SetNeedsBoxFragment()`来标志该span需要fragment。由于NGPhysicalBoxFragment 要在知道孩子节点和size后才可创建，可以先调用NGInlineLayoutStateStack::AddBoxFragmentPlaceholder()，创建一个place holder。然后在加入children，然后再决定他们的位置。当所有child和他们的位置都确定后，通过`NGInlineLayoutStateStack::CreateBoxFragments()`创建NGPhysicalBoxFragment，然后把孩子加进去。

## 杂项

### baseline

如何计算：
1. user of baseline 通过调用 `NGConstraintSpaceBuilder::AddBaselineRequest()`获取他们需要的type的baseline。
2. 调用`NGLayoutInputNode::Layout()`，执行正确的layout算法
3. 不同的layout算法根据type计算baseline
4. user通过`NGPhysicalBoxFragment::Baseline()`或者`NGBoxFragment::BaselineMetrics()`获得结果。
Algorithms负责检查`NGConstraintSpace::BaselineRequests()`，计算requested baseline，通过调用`NGFragmentBuilder::AddBaseline()`把结果加入`NGPhysicalBoxFragment`。
NGBaselineRequest 由 NGBaselineAlgorithmType 和 FontBaseline组成.
大多数情况下，algorithms决定由哪个box提供baseline，并把request委托给它。
只有两种baseline：拼音和表意。由writing-mode决定的。
后续：支持更多的baseline，由line box和text的fragment计算出来。。

### Bidirectional Text

[uinicode这个算法](http://unicode.org/reports/tr9/)定义了bidirectional text的算法。
主要实现逻辑在NGBidiParagraph, 其实是对[ICU BiDi](http://userguide.icu-project.org/transforms/bidi)的一层封装。
主要又两部分组成：
1. line breaking之前：Segmenting text and resolve embedding levels 
2. line breaking之后：Reorder text 


## inline layout的堆栈

NG的inline layout
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



## NGInlineBoxState

代表在NGInlineLayoutAlgorithm layout时的当前box状态信息。其中metrics是所有子孙节点metrics的合并，如lineheight等会影响这个值。NGInlineBoxState::ComputeTextMetrics里进行合并。
在NGInlineLayoutAlgorithm::CreateLine中，会用到`box_states_`，是一个NGInlineBoxState的stack。NGInlineLayoutAlgorithm::CreateLine开始时会根据当前节点的style创建NGInlineBoxState，然后遍历`line_items`，对不同的item进行不同的处理，若openTag，就创建一个新的state压栈，closeTag及pop出栈。遇到text，不新建，但会通过ComputeTextMetrics，把text的metrics合并上去。最后会根据最后一个state的metrics计算垂直偏移量。
