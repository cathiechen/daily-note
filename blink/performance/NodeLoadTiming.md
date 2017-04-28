# NodeLoadTiming

## 意图
- 一个node从下载到显示的时间戳。 让页面开发者，全面掌握node的情况。 可用于解决类似小程序场景的webview切换问题。

## 详细
- 每个阶段都会产生一个event，如果页面有监听，fire
- 阶段：
	- dom begin： 解析到开始标签
	- dom finished： 这个节点解析到结束标签
	- js applied： 这个节点相关的js已经下载，并运行。js 可以指定，默认值=node之前的所有js文件
	- css applied： 这个节点相关的css已经下载，并应用。css可以被指定，默认值=node之前的所有css文件
	- img loaded： 这个节点内的所有img元素已下载完毕
	- final layout： 以上条件全部满足后的，第一次layout
	- finial paint： 以上条件全部满足后的，第一次paint
	- finish： 表示该node已经以最后的形态显示到屏幕

## 后续
- 这个跟[ElementTiming](https://github.com/w3c/charter-webperf/issues/30) :(