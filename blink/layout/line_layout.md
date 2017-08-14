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
