#The assert issue of LayoutText
##Assert的地方

	  // We shouldn't change our mind once we "know".
	  ASSERT(!m_knownToHaveNoOverflowAndNoFallbackFonts ||
	         (fallbackFonts.isEmpty() && glyphOverflow.isApproximatelyZero()));
	  m_knownToHaveNoOverflowAndNoFallbackFonts =
	      fallbackFonts.isEmpty() && glyphOverflow.isApproximatelyZero();


disscussed in [codereview](https://codereview.chromium.org/2683553002/)

##触发条件
- The style of LayoutText is only changed by `setStyleInternal()`, e.g. rotated the autosized page.
- Triggle `LayoutText::computePreferredLogicalWidths()` during `layout()`. e.g. page with <ruby>, [example page](./glyphOverflow.html)
- Some text change from `glyphOverflow.isZero` to `!glyphOverflow.isZero`, see the log below:

Log of 'n' with 'font-family:Times New Roman;'，font-size:12px~30px:

	
		glyphOverflow=(0.000000, 0.259766, 0.000000, 0.000000)
		glyphOverflow=(0.000000, 0.614746, 0.000000, 0.000000)
		glyphOverflow=(0.000000, 0.000000, 0.000000, 0.000000)
		glyphOverflow=(0.000000, 0.324707, 0.000000, 0.000000)
		glyphOverflow=(0.000000, 0.000000, 0.000000, 0.000000)
		glyphOverflow=(0.000000, 0.034668, 0.000000, 0.000000)
		glyphOverflow=(0.000000, 0.389648, 0.000000, 0.000000)
		glyphOverflow=(0.000000, 0.000000, 0.000000, 0.000000)
		glyphOverflow=(0.000000, 0.099609, 0.000000, 0.000000)
		glyphOverflow=(0.000000, 0.000000, 0.000000, 0.000000)
		glyphOverflow=(0.000000, 0.000000, 0.000000, 0.000000)
		glyphOverflow=(0.000000, 0.164551, 0.000000, 0.000000)
		glyphOverflow=(0.000000, 0.000000, 0.000000, 0.000000)
		glyphOverflow=(0.000000, 0.000000, 0.000000, 0.000000)
		glyphOverflow=(0.000000, 0.229492, 0.000000, 0.000000)
		glyphOverflow=(0.000000, 0.000000, 0.000000, 0.000000)
		glyphOverflow=(0.000000, 0.000000, 0.000000, 0.000000)
		glyphOverflow=(0.000000, 0.000000, 0.000000, 0.000000)
		glyphOverflow=(0.000000, 0.000000, 0.000000, 0.000000)


- [This page](./ruby2.html) can reproduce it every time rotated the screen.

##思路
- 说真的，一开始是懵逼的！但看这变量`m_knownToHaveNoOverflowAndNoFallbackFonts`用的地方很少啊，所以gdb了一下，得出结论: 嗯，autosize的时候应该把它设置成false。但还是懵逼状态啊。
- 然后，看到谷歌的大神pdr评论是否跟`glyphOverflow`有关？是否是因为`m_knownToHaveNoOverflowAndNoFallbackFonts`在旋转屏前后，由false变成了true？能否给个测试用例？事实证明他的推断都是正确的。orz
- 带着我对大神崇高的敬意，开始搞test case. 但是好奇怪，明明在手机上跑得好好的页面，换成unit test就不行了。啊？还跟设置的页面宽度有关，还必须是360*640的屏幕！我不会告诉你，那个测试用例我搞了一天，而且是加班到11点才搞出来的。搞完test case， 然后手贱回了一句fallbackFonts不是空的.(啪啪地打脸，事实证明，是空的！哭晕在厕所。。。)
- wait a minute! 但是神马是fallbackFonts?! 神马是glyphOverflow?! 搞了快一周了，还是懵逼状态啊。所以花了一两天时间去学习font相关的知识。看完，回到bug，还是有点懵，当然这是很有用的;）
- 还是用可以稳定重现的s4改bug。手贱，把出现assert的内容换了一下。哇塞，还有差异诶。然后，就在pin yin里面筛选字母。发现有字母n和y，就会导致assert。所以，选定n，放到rubytext里面。
- 为啥一定要在ruby里面才出现呢？放到普通text会怎样。所以写了全篇由n组成页面，发现没啥卵用。看了一下log，md，根本没来`computePreferredLogicalWidths`。
- 另一根神经又在想，要不把n的各种font-size的`glyphOverflow`打出来吧。[here](./ruby7.html)
- 咦，为啥这里可以打出来？嗯，加了font-size会触发`computePreferredLogicalWidths`,所以又写了[这个页面](./glyphOverflow.html)。稳定重现，[开心脸]
- 最后，基本上分析出原因了。由于autosize的横竖屏切换时候，会重新计算font-size，且通过`setStyleInternal`设置的，这个设置和`styleDidChanged`不一样，不会设置`m_knownToHaveNoOverflowAndNoFallbackFonts=false`.由于字体变化了，`glyphOverflow`也有可能变化，所以有一定概率引起上面那个assert通不过的问题。
- 但是，我这个对自己有要求的宝宝。必须把之前那个sb test case改成全屏是n的test case。问题来了，tm又不能在unit test上重现。说这个诡异的事件快把我逼疯了，是一点也不夸张的事情。就在快崩溃或者已经崩溃的时候，我想起了fontcache的key不只跟description有关，还跟font-family有关！（所以说，学习font是有用的嘛，哈哈）。嗯，`fontcache`是什么鬼，全篇没出现过，快结束了，突然来这么一出？！哈哈，好啦，字体相关的总结看下篇日志吧。然后unit test加了font-family，基本可以稳定重现。但是，打出来的glyphOverflow和手机页面上的不同！为啥不同呢？我也不知道！

##解决
- 当然是跟刚开始那个方法一样，applymultiplier时，设置layouttext的`m_knownToHaveNoOverflowAndNoFallbackFonts` false。
- 这时候的感觉和刚开始蒙对改法的感觉是完全不一样的，快累死姐了！连着好几天11点左右到家。我爸妈都奇怪了：你不是要干嘛干嘛吗？怎么还搞这么晚？都来深圳了，还跟你一天讲话超不过五句？！嗯，只好告诉他们，明天就好，明天就好了...

##总结
- 这次bug test case对理解代码有很大的帮助，基本上是一边写test case，一边冒问题，然后才把这个bug搞懂的;)
