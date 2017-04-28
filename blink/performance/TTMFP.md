# [Time to First Meaningful Paint](https://docs.google.com/document/d/1BR94tJdZLsin5poeet0XoTW60M0SjvOJQttKT-JK8HI/edit#heading=h.tdqghbi9ia5d)

## 背景
First Meaningful Paint是指页面上的基本内容第一次出现在屏幕上的时间（主要是首屏）。这个跟之前理解的有点不一样：之前觉得是要在首屏前，做一些操作，让首屏更快；但这个只是记录首屏的时间。
不同场景下的FMP:

- blog: headline + above-the-flod text(可视的text)
- search: search results
- image critical

该文档准确率达77%。

## design
### 1. 基本方法：计数layout objects
- object 的增数和FMP强相关。
- [LayoutAnalyzer](https://cs.chromium.org/chromium/src/third_party/WebKit/Source/core/layout/LayoutAnalyzer.h?sq=package:chromium&type=cs)可以收集这几个数字，并通过trace event传出来。
- FMP = paint that follows biggest layout change

### 2. 针对其它情况进一步优化
- 长页面
	- 长页面有可能出现排版显示范围之外的地方的layout 数量变化比首屏多
	- 所以引进了layout significance
		- layout significance  number of layout objects added / max(1, page height/screen height)
		- 这个很好地降低了首屏之外元素的影响。
- web font
	- 对于需要下载字体的页面，先用fallback font的 metrics排版，但不画出来，所谓[font block preiod](https://tabatkins.github.io/specs/css-font-display/#font-block-period)。
	- 推迟到font displayed后，再count；or 3s timeout exceeded.
	- 过滤：超过200个字的font才会block count
	
### 3. 警告
- 只针对新增object，不针对recalculate style引起layout的object
- 首屏大图片的页面，不准
- 先display:none，在渐变显示出来的页面，不准
- 依赖screen size

## evaluation（评估？）
一些测试数据，准确率达77.3%

## 实现计划
1. trace-baseed implementation：基于tracing，完成处理数据脚本
2. UMA：收集并上报现实用户使用的数据

## 实现现状
1. 完成trace-based implementation
	1. 使用：record a trace, make sure thest two catefories enabled: blink.user_timing, loading
	2. firstPaintMetric
	3. 还可以通过脚本计算出结果：`third_party/catapult/tracing/bin/run_metric trace.json loadingMetric`
2. [UMA](https://docs.google.com/document/d/1vi-5Oa7EFfcJXug8x-pxVw343GNr3SZmKF39DSuB9rw/edit#heading=h.7rtig6exej6x):应该基本实现了
	1. 背景
		1. trace-based有自己的劣势，不是来自用户的真是数据
		2. UMA: 基于用户使用的真实数据，cool！
	2. design
		1. 如何计算layout significance? 需要的数据：
			1. 新增obj数量：在FrameView里增加一个变量,保存counter，只增不减。
			2. pageheight, screen height： FrameView中现成
			3. number of blank character： FontFaceSet现成
		2. 收集数据
			1. [PaintTiming](https://cs.chromium.org/chromium/src/third_party/WebKit/Source/core/paint/PaintTiming.h?sq=package:chromium)里包含一个新的class: FirstMeaningfulPaintDetector
				1. layout的时候，把上面的3个数据传给FirstMeaningfulPaintDetector
				2. FirstMeaningfulPaintDetector，通过这些数据，计算出layout singnificace, 如果达到当前最大，设置m_nextPaintIsMeaningful = true
				3. paint的时候，若m_nextPaintIsMeaningful = true，保存当前的timerstamp
				4. load结束的时候，FirstMeaningfulPaintDetector把FMP传递给[PaintTiming](https://cs.chromium.org/chromium/src/third_party/WebKit/Source/core/paint/PaintTiming.h?sq=package:chromium)，通过`PageLoadMetrics`上报
		3. 什么时候结束监控？
			1. trace based通过扫描所有发出的event，获得
			2. UMA，由于存在js动态添加object，所以需要主动结束
			3. 结束的标志：
				1. onload event([performance.timing.loadEventEnd](https://www.w3.org/TR/navigation-timing/#dom-performancetiming-loadend))，存在onload在FMP之前结束的情况，故不可用
				2. [fully loaded](https://sites.google.com/a/webpagetest.org/docs/using-webpagetest/metrics)——document complete之后2s内没有网络活动。但long-polling的网站也会不适用。目前用的是这个。 fully loaded，结束监控。
		4. 降噪
			1. fully loaded之前退出
			2. 用户行为干预，比如 操作menu什么的...用户干预，不上报。[如何判断用户干预？](https://cs.chromium.org/chromium/src/chrome/browser/page_load_metrics/page_load_metrics_observer.h)

## references
- [讨论](https://groups.google.com/a/chromium.org/forum/#!msg/loading-dev/GZyyd9IRqCU/ny_Tth9OBwAJ)