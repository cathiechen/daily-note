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
