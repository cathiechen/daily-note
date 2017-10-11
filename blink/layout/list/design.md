# 排版修改应该考虑到的地方

排版涉及很多，修改问题的时候总是想不全。现在先把遇到的记下来，以后继续补充。

1. 正确性，最常见的静态页面的显示正确性。
2. 节点自身css发生变化，比如outside变成inside。
3. dom树发生变化: 
	- 空
	- 新增inline、新增block
	- 删除一个、删除全部
	- 改： block改成inline，其实这个是css，但影响到layout tree，先放这边
3. css环境发生变化：这个很复杂，指的是父亲节点或孩子节点的css发生变化的情况 。主要可以考虑：float\overflow\positioned\lineheight\writingmode\verticalAlign
4. 横竖屏切换
