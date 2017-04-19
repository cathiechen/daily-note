# ruby

### 是什么及怎么用：
1. ruby 让文字可以分上下两层显示，汉语中的拼音。日语好像是经常用到。
2. 这是最基本的用法：`<ruby><rb>拼音</rb><rt style="font-size:20px">pin yin</rt></ruby>`

### layout tree:

	layoutRuby(block/inline)
        layoutRubyRun(inline-block)（anonymous）
	        layoutRubyBase(block)（anonymous）
	            layoutInline(<rb>)
	                text
	        layoutRubyText(block, <rt>)
	            text

特点：
1. rubyRun的构造，addchild之前必须有run
2. rubybase的构造

### layout：

1. 顺序：
	1. layout ruby( inline )
		1. layout rubyrun( inline box )
			1. layout rubytext( layoutSpecialExcludedChild() )
			2. layout rubyBase
		2. position rubytext in rubyrun
		3. finish rubyrun layout as inline box
	2. finish ruby layout as inline
2. 特点：
	1. 主要还是用block的layout，但rubytext的触发和position rubytext都是特殊处理的


### 问题：
- 无法被autosize放大
	- 原因：run的inline-block会自己创建cluster，所以不会用父cluster的multiplier；
	- 解决思路：跟table的差不多。
		- 在处理ruby的时候，把inner block inflate掉。必须在layout run之前进行，因为run是inline-block，所以它的宽度是由内容决定的。如果在layout run之后autosize，run的宽度不够，会出现折行的问题。
		- skip run，不skip的话，会创建cluster
- 横竖屏切换的排版问题
	- 原因：字体大小改变后，需要通知父亲节点重新计算preferredWidth，但一般情况下，父亲节点都是block的，且inline-block一般会创建自己的cluster，一般不会被放大字体，所以只要mark text的inline parent重新计算preferredWidth即可。但ruby有一个inline-block被skip掉了。所以横竖屏切换时，字体发生变化，但inline-block没被通知到重新计算preferredWidth，所以，横竖屏的时候就悲剧了
	- 解决思路：在mark preferredWidthDirty时，判断若已经skip inlineblock，则让这个mark MarkContainerChain:)




