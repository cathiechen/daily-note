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
