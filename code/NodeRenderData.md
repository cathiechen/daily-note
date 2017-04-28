# 一些在现有工程里面添加新功能的方法

## NodeRenderData
- [讨论](https://groups.google.com/a/chromium.org/forum/#!topic/style-dev/iLGP_mMZdjE): 虽然不是很懂，大概意思是把Node里面的LayoutObject改成NodeRenderData。 由于layoutObject不稳定，删除或重建什么，所有不保存object，保存一些render相关的数据到Node，这些数据构造成一个叫NodeRenderData的类。
	- 数据包含：
		- All the PseudoElements currently in ElementRareData.
		- All the RestyleFlags, (This will use a bit more memory but it's probably fine.)
		- ComputedStyle (so we can keep the style around even if we destroy the layout tree).
		- Pseudo styles for this node (this impacts style sharing so it'll require more thought).
		- List of fragments associated with this Node, currently this is LayoutObject.
		- We could store the pending style for rebuildLayoutTree here and remove the HashMap.
	- 以后的流程可能变成：
		- recalcStyle(css + dom) --> NodeRenderData
		- layout(NodeRenderData) --> fragment
		- paint(fragment + computedStyle) --> displayList
- 实现:
	- [阶段1](https://codereview.chromium.org/2814603002/)： 1. 创建NodeLayoutData类(后来改成NodeRenderData)； 2. 添加LayoutObject到NodeLayoutData里面
	- [阶段2](https://codereview.chromium.org/2821193003/)： 把document里面的NonAttachedStyle加到NodeLayoutData里面
	- 改NodeLayoutData为NodeRenderData
- 总结： 
	- 她不是一次性把所有要实现的功能都加进去的，先提交基本的框架，然后再丰富。这样更有层次性。