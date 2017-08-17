# list marker 定位方案重构

## 当前list marker 定位的方案以及存在的问题
  - 创建一个inline的marker
  - 把marker加上layout tree，并更新marker的margin和content
  - computeoverflow（li也就是layout完所有孩子后），重新更正marker的水平位置
  - 这个方案是：把marker加入content flow里，省略了垂直位置的的计算，只要计算水平位置，但存在以下问题
    - !overflow: visibility             
    - nested li
    - 空content高度
    - 插入inline object会影响li的高度，overflow无法正常显示
    - 插入li，影响高度和换行

## W3C对list的规定

### [CSS2.2 lists翻译](https://www.w3.org/TR/CSS22/generate.html#listso)

CSS2.2 为li的content创建一个主块，根据`list-style-typ`和`list-style-image`的值，会创建一个marker box，以显示这个element是li

list属性描述li的基本样式：允许指定marker的type（image， glyph， number），以及指定marker相对于主块的位置(outside, inside), 不允许指定marker的样式和相对位置。

background，只作用与主块，marker的是透明的。

- `list-style-position`
        - outside: marker-box在主块之外：
                - 没有定义与float相邻的定位位置
                - 没有定义marker box具体的位置，和paint order
                - li dir：ltr，marker在左边；反之，右边
                - marker box固定与主块的border，不随着主块内容滚动而滚动
                - 若主块的overflow非visible，有可能导致marker无法显示（要改）
                - marker box的大小和内容有可能影响主块或其第一个line box的高度。某些情况下，可能引起新行的创建
        - inside: 作为主块的第一个line box，位于`::before`之前

虽然没有规定marker box的位置，及显示，但从最新的草案中可以看到，以后的趋势是让任何一个element都可以成为marker。
虽然还没成为标准，`::marker`（标准）, `position: marker`（非标准，没人知道）等趋势。
https://www.w3.org/TR/css-pseudo-4/#marker-pseudo


## 新方案

- 创建marker： 
	- inside: inilne; 
	- outside: absolute
- 添加marker到layout tree： 
	- inside： parent of first line box; 
	- outside: li
- layout marker: 
	- inside: 设置宽高即可；
	- outside: 除了设置宽高，还要计算static position, 由于abs是在normal child之后layout，此时可以对齐block方向的位置
		- 如何对齐？ 找到第一个text，marker偏移text的parent到li的偏移，在加上baseline的差异

## 需要修改的地方
- 创建marker
- add to layout tree
- layout: static position, block方向对齐
- listitemstyledidchange: 设置abs属性
- 删除：li computeOverflow时的处理要删除。
- 由于marker是继承box的，inside的话，setinline(true)就有inline属性了；outside：是abs，是否需要改成block的？
- 很多重写的函数，需要添加outside的处理
