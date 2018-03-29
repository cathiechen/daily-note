# inline layout

line layout 一直以来是个盲区，最近顶着硕大的压力，终于看到了一些东西，记录下来。

## 流程
比block的复杂，line在layout之前，需要做一些处理

- line是如何layout出来的：
	- 分行和runs
		- 利用传说中的Linebreaker，把inline元素和text断行的位置确定，并创建run，把这些run寄存到一个list中
	- createLineBoxesFromBidiRuns
		- constructLine，首先构建linebox。遍历run list，并向上找父节点，创建parentBox
			- 对于每个block，都会创建一个rootinlinebox
			- inline创建inlineflowbox
			- text run创建linebox
		- 把这些box的位置宽高，以及overflow计算清楚
			- computeinlinedirectionpositionforline，水平方向
			- computeblockdirectionpositionforline，垂直方向
			- computeoverflow
	- 流程：
		- 所有的bidi都保存在一个叫resolver的地方
		- 确定这个line开始的bidi，利用linebreaker确定endline。把这些bidi runs 保存在bidiruns中
		- 创建一个rootinlinebox，代表一个line。每个 bidi run 对应一个linebox，对这些line boxes进行水平和垂直对齐。在垂直排版时，lineheight会影响到line的高度，具体见：rootinlinebox::ascentanddescentforbox.
		- 排版完后，clear bidiruns。
		- 获取下一行的bidiruns。重复

这样就排好一行了，如果还有下一行，继续，分行+layout

## 重要数据结构

- runs
- inlinebox, 基类，一般对应一段文字，
	- inlineflowbox，一般对应inline元素，存在overflow
		- rootinlinebox，对应一个block，包含换行信息和overflow


## 各种is

- `IsLayoutInline()`: `EDisplay::kInline:`, 跟display:inline相关，是一种layoutobject， `LayoutInline : LayoutBoxModelObject : public LayoutObject`
	- `EDisplay::kInlineBlock` 具有这种display的元素不会被创建成LayoutInline，LayoutBlock instead:)
- `IsInline()`: LayoutObject的一种属性
	- `display == EDisplay::kInline`
	- `IsDisplayReplacedType`
```  
  static bool IsDisplayReplacedType(EDisplay display) {
    return display == EDisplay::kInlineBlock ||
           display == EDisplay::kWebkitInlineBox ||
           display == EDisplay::kInlineFlex ||
           display == EDisplay::kInlineTable ||
           display == EDisplay::kInlineGrid;
  }
```

- 总结：`IsLayoutInline`就是inline是天生具有不换行的属性那种。`isInline`它本身可以是block但又不换行，具有inline的特性。


## layout目录下的结构变化

- api: 多了一个api文件夹，是layout对外的接口，也就是，document之类的不会直接调用layoutobject，只会调用layoutitem
- line: inlineBox之类的, 跟line相关的操作，除了line内部调用，edit也会用到，什么nextleafchild之类的


## alwaysRequiresLineBox

```
<html>
<head>
    <style>
        .generate:before {content: 'before';}
        ul {border: 1px green solid;}
        span {border: 1px red solid;}

    </style>
</head>
<body><div><span><div>item</div></span></div></body>
</html>
```

layout tree:

- layoutBlock
	- anonymousBlock1
		- first part of span1
	- anonymousblock2
		- div
	- anonymousblock3
		- second part of span2
在实现list marker的过程中，遇到一个奇怪的问题，上面那个例子，如果span存在border，则会多出两个带border的行，总共显示3行; 若没设置border，则没用空白行，总共显示1行！所以，特地调查了一下。

原因是：在nextLineBreak过程中，有个检验requiresLineBox的逻辑，由于span1是一个没有孩子节点的inline，所以通过校验其是否需要创建linebox来决定是否找到了linebreaker。alwaysRequiresLineBox 验证inline是否有pandding或border，margin，若有返回true；若没有，返回false。对于不需要创建的linebox的元素，会导致linebreak找不到下个linebreak的元素，这样就不会由后面的创建linebox，rootinlinebox，并layout这些linebox。具体调用堆栈如下：

```
>       minichrome.exe!blink::LayoutBoxModelObject::hasInlineDirectionBordersPaddingOrMargin() 行 176   C++
        minichrome.exe!blink::alwaysRequiresLineBox(blink::LayoutObject * flow) 行 193  C++
        minichrome.exe!blink::requiresLineBox(const blink::InlineIterator & it, const blink::LineInfo & lineInfo, blink::WhitespacePosition whitespacePosition) 行 201  C++
        minichrome.exe!blink::LineBreaker::skipLeadingWhitespace(blink::BidiResolver<blink::InlineIterator,blink::BidiRun> & resolver, blink::LineInfo & lineInfo, blink::FloatingObject * lastFloatFromPreviousLine, blink::LineWidth & width) 行 33   C++
        minichrome.exe!blink::LineBreaker::nextLineBreak(blink::BidiResolver<blink::InlineIterator,blink::BidiRun> & resolver, blink::LineInfo & lineInfo, blink::LayoutTextInfo & layoutTextInfo, blink::FloatingObject * lastFloatFromPreviousLine, WTF::Vector<blink::WordMeasurement,64,WTF::DefaultAllocator> & wordMeasurements) 行 70    C++
        minichrome.exe!blink::LayoutBlockFlow::layoutRunsAndFloatsInRange(blink::LineLayoutState & layoutState, blink::BidiResolver<blink::InlineIterator,blink::BidiRun> & resolver, const blink::InlineIterator & cleanLineStart, const blink::BidiStatus & cleanLineBidiStatus) 行 812       C++

```
为存在border的span创建linebox
```
>	minichrome.exe!blink::BreakingContext::handleEmptyInline() 行 450	C++
 	minichrome.exe!blink::LineBreaker::nextLineBreak(blink::BidiResolver<blink::InlineIterator,blink::BidiRun> & resolver, blink::LineInfo & lineInfo, blink::LayoutTextInfo & layoutTextInfo, blink::FloatingObject * lastFloatFromPreviousLine, WTF::Vector<blink::WordMeasurement,64,WTF::DefaultAllocator> & wordMeasurements) 行 84	C++
 	minichrome.exe!blink::LayoutBlockFlow::layoutRunsAndFloatsInRange(blink::LineLayoutState & layoutState, blink::BidiResolver<blink::InlineIterator,blink::BidiRun> & resolver, const blink::InlineIterator & cleanLineStart, const blink::BidiStatus & cleanLineBidiStatus) 行 812	C++
```


## AlignBoxInBlockDirection

这里是垂直方向上排版inlinebox的。以前一直以为rootinlinebox是inlinebox的parent，inlinebox的定位是基于rootinlinebox的logicalTop的。但事实上并不是这样，rootinlinebox并不会拥有任何inlinebox，它只是负责排版inlineboxes。

- ComputeLogicalBoxHeight，作用是递归计算所有inlinebox的ascent、descent等，还要计算出max相关值
	- 第一步，`RootInlineBox::VerticalPositionForBox`：计算inlinebox和rootinlinebox的baseline之间的距离，并把这个距离设置为inlinebox的logicalTop
	- 第二步，`RootInlineBox::AscentAndDescentForBox`：计算每个inlinebox的ascent和descent。ascent + descent = lineheight.
		- inlinebox字体的ascent、descent是怎样算出来的？fontdata里有个叫fontmetrics的类，它可以计算出这个字体的ascent, height和linespacing。linespacing比fontmetric的height要大一点，包含一行中字体上下的空隙。ascent = 上面空隙 + fontmetrics.ascent. descent =  linespacing - ascent. rootinlinebox的baselinetype会影响到ascent。
		- 如果inlinebox的元素不是text，那么ascent=box->baseline; descent=lineheight - ascent. 所以，marker作为一个inline，它的ascent就是baseline。
	- 第三步，计算maxPositionTop，maxPositionBottom, maxAscent, maxDescent等
	- 递归对其孩子进行以上计算
- `InlineFlowBox::PlaceBoxesInBlockDirection`
	- 设置rootinlinebox的 logicalTop为“blockflow的top” + maxAscent - fontMetric.Ascent. 如果孩子inlinebox的lineheight和baseline与rootinlinebox相同，则，设置其logicalTop未这个高度。
	- 根据verticalAlign设置各个inlinebox的logicalTop：
		- 如果inlinebox的lineheight和baseline与rootinlinebox相同，则设置inlinebox的logicalTop为rootinlinebox的logicaltop。(marker就可以走这个流程)
		- 如果verticalAlign：top，inlinebox的logicalTop设置为：“blockflow的top” 
		- bottom，设置为：top + maxHeight - curr->lineHeight;
		- 其他，设置为：top + maxAscent + curr->logicaltop() - curr->baseline(), 这里其实跟rootinlinebox是一样的，cur->logicalTop就是第一步设置的basline之间的距离
	- 计算newLogicalTop
		- 文字，或inlineflow：调整一下inlinebox的logicaltop，`new_logical_top += curr->BaselinePosition(baseline_type) - font_metrics.Ascent(baseline_type);`，在上一步中logcaltop = top + maxAscent + curr->logicaltop() - curr->baseline()，所以经过调整后，new_logical_top = top + maxAscent + curr->logicaltop() - fontMetrics.Ascent()。最后，再减去border和padding
		- br，new_logical_top += marginTop
	- 递归计算孩子节点
	- 处理lineTop和lineBottom
- 设置lineTop和lineBottom
- 修改top为 top+=maxHeight
- 完成


- 关于marker的垂直对齐方案：rootInlineBox的logicalTop：“blockflow的top” + maxAscent - rootInlineBoxe的fontMetric.Ascent. 所以，像marker这种想把自己与该行对齐的需求，marker的inineBox的logicalTop应该设置成： “blockflow的top” + maxAscent - marker InlineBox的fontMetric.Ascent. 所以，marker logicalTop = rootInlineBox logicalTop + rootInlineBox fontMetric.Ascent - marker fontMetric.Ascent。


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

## baseline

从`AlignBoxInBlockDirection`那段可以知道：rootinlinebox并不会拥有任何inlinebox，它只是负责排版inlineboxes，它的logicalTop=maxAscent - fontMetrics.ascent. 它代表行的信息：收集maxAscent, maxDescent, maxPositionTop，maxPositionBottom. 

baseline 和 ascent不可描述的关系：对这两个概念一值模糊，其实ascent是对font的，baseline对于inlinebox。baseline=ascent + (lineHeight - fontmetrics.height) / 2. 也就是说，baseline会带上lineheight的信息。lineheight就是css设置的那个。baseline是一段距离，linetop到position文字的ascent的距离。图片的话，就是：图片的高度+上下的margin.


FirstLineBoxBaseline：FirstLineBox.LogicalTop() + FontMetrics.Ascent。其实，这是个位置。

当lineHeight是自己产生的时候，FirstLineBoxBaseline 和 baseline是一样的。但由孩子节点产生时，这两个就不一样了。


inlinebox的LogicalFrameRect: 不管lineheight设置多大，不会影响inlinebox高度，只会影响top. 这个应该是内容有多大，就是多大。

