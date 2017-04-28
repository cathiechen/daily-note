# [Navigation Timing](https://www.w3.org/TR/navigation-timing)

基于网络角度量化从网页打开到网页显示过程中的耗时。补一个web preformance monitor的构成图：
![](https://w3c.github.io/perf-timing-primer/images/W3C-WebPerf-deps-graph.png)

web-perf的整体思想是把网页加载过程中，针对不同阶段，元素，操作等建立一个时间度量的标准。

## PerformanceTiming

	interface PerformanceTiming {
	  readonly attribute unsigned long long navigationStart;
	  readonly attribute unsigned long long unloadEventStart;
	  readonly attribute unsigned long long unloadEventEnd;
	  readonly attribute unsigned long long redirectStart;
	  readonly attribute unsigned long long redirectEnd;
	  readonly attribute unsigned long long fetchStart;
	  readonly attribute unsigned long long domainLookupStart;
	  readonly attribute unsigned long long domainLookupEnd;
	  readonly attribute unsigned long long connectStart;
	  readonly attribute unsigned long long connectEnd;
	  readonly attribute unsigned long long secureConnectionStart;
	  readonly attribute unsigned long long requestStart;
	  readonly attribute unsigned long long responseStart;
	  readonly attribute unsigned long long responseEnd;
	  readonly attribute unsigned long long domLoading;
	  readonly attribute unsigned long long domInteractive;
	  readonly attribute unsigned long long domContentLoadedEventStart;
	  readonly attribute unsigned long long domContentLoadedEventEnd;
	  readonly attribute unsigned long long domComplete;
	  readonly attribute unsigned long long loadEventStart;
	  readonly attribute unsigned long long loadEventEnd;
	};


-  navigationStart: 开始，上个页面unload时，获fetchStart
-  unloadEventStart
	- 与上个页面same origin： unload上个页面时
	- 没有上个页面或different origin: 0
-  unloadEventEnd
 	- same origin： unload上个页面时
	- no / different origin: 0
-  redirectStart
	- same: 开始获取redirect时
	- 否则：0
-  redirectEnd
	- same：获取redirect结束时
	- 否则：0
-  fetchStart
	-  开始check cache时
	-  或开始fetch资源时
-  domainLookupStart
	-  浏览器开始域名查找时
	-  fetchStart（长连接，cache，local）
-  domainLookupEnd
	-  域名查找结束时
	-  fetchStart（长连接，cache，local）
-  connectStart
	-  开始与服务器建立链接时
	-  domainLookupEnd
-  secureConnectionStart
	-  https：开始握手时间
	-  非https： 0
-  connectEnd
	-  链接建立结束后
	-  domainLookupEnd
	-  若新开链接，取相应的connectEnd
	-  这里包含建立链接、ssl握手、socks认证的时间间隔
-  requestStart
	-  request开始时
	-  若重发，使用新的request的start时间
	-  没有requestEnd，原因：
		-  requestEnd不代表网络传输结束，意义不大
		-  由于http层的封装，获取requestEnd特别耗时
-  responseStart
	-  收到第一个字节的时间
-  responseEnd
	-  收到最后一个字节的时间，或transport connection关闭的时间。
-  domLoading
	-  document的readiness设置成"loading"时(readiness解释，见下文)
-  domInteractive
	-  document的readiness设置成"interactive"时
-  domContentLoadedEventStart
	-  DomContentLoaded event fired
-  domContentLoadedEventEnd
	-  处理结束
-  domComplete
	-  document的readiness设置成"complete"时
-  loadEventStart
	-  load event fired
-  loadEventEnd
	-  load event complete

## PerformanceNavigation
	interface PerformanceNavigation {
	  const unsigned short TYPE_NAVIGATE = 0;
	  const unsigned short TYPE_RELOAD = 1;
	  const unsigned short TYPE_BACK_FORWARD = 2;
	  const unsigned short TYPE_RESERVED = 255;
	  readonly attribute unsigned short type;
	  readonly attribute unsigned short redirectCount;
	};

- type: 最后一个非redirect页面的类型: navigate, reload, back_forward, reserved
- redirectCount: 中间跳转了几个页面

## window.performance
	interface Performance {
	  readonly attribute PerformanceTiming timing;
	  readonly attribute PerformanceNavigation navigation;
	};
	
	partial interface Window {
	  [Replaceable] readonly attribute Performance performance;
	};

## process

![](http://i.imgur.com/65P9Qhk.png)

## 附录：
### [如何结束document的parse](https://www.w3.org/TR/html5/syntax.html#the-end)：
document parse结束的流程，各种状态的变化：

1. readiness 设置成 "interactive"
2. pop all open elements
3. 执行attach到parse end的js， [list of scripts that will execute when the document has finished parsing](https://www.w3.org/TR/html5/scripting-1.html#list-of-scripts-that-will-execute-when-the-document-has-finished-parsing)
4. fire an event: **DOMContentLoaded** 
5. 执行需要被尽快执行的js，[set of scripts that will execute as soon as possible](https://www.w3.org/TR/html5/scripting-1.html#set-of-scripts-that-will-execute-as-soon-as-possible)和[list of scripts that will execute in order as soon as possible](https://www.w3.org/TR/html5/scripting-1.html#list-of-scripts-that-will-execute-in-order-as-soon-as-possible)
6. 继续执行event loop， 直到没有delays the load event
7. 发起一个task
	1. readiness设置成"complete"
	2. 为window创建一个**load** event， 不bubble，不可取消
8. 发起另一个task
	1. pageShow设置成true
	2. document、window fired pageShow事件
9. 为[pending application cache download process tasks](https://www.w3.org/TR/html5/browsers.html#pending-application-cache-download-process-tasks)创建tasks
10. 若 [print when loaded](https://www.w3.org/TR/html5/webappapis.html#print-when-loaded), print
11. 可以开始 post-load tasks了
12. 发起mark document completely loaded的task

### chrome打开github.com的数据

	window.performance：

	navigationStart	1490868289580 //后面的unload事件被移动到responseStart之后
	redirectEnd	0
	redirectStart	0
	fetchStart	1490868289580
	domainLookupStart	1490868289580
	domainLookupEnd	1490868289580
	connectStart	1490868289582
	secureConnectionStart	1490868289855
	connectEnd	1490868290125
	
	requestStart	1490868290125
	responseStart	1490868290496
	unloadEventStart	1490868290499 //unload事件处理，跟白屏处理有关？
	unloadEventEnd	1490868290499
	responseEnd	1490868290500
	domLoading	1490868290502
	domInteractive	1490868290505
	domContentLoadedEventStart	1490868290505
	domContentLoadedEventEnd	1490868290505
	domComplete	1490868290630
	loadEventStart	1490868290630
	loadEventEnd	1490868290630

## chromium的实现

web层的封装[WebPerformance.cpp](https://cs.chromium.org/chromium/src/third_party/WebKit/Source/web/WebPerformance.cpp?type=cs&l=113)。当然，因为是对外的接口，所以还需要进行ID和string的转换。

- ResourceLoadTiming：
	- m_requestTime;
	- m_proxyStart;
	- m_proxyEnd;
	- m_dnsStart;
	- m_dnsEnd;
	- m_connectStart;
	- m_connectEnd;
	- m_workerStart;
	- m_workerReady;
	- m_sendStart;
	- m_sendEnd;
	- m_receiveHeadersEnd; //这个是responseStart
	- m_sslStart;
	- m_sslEnd;
	- m_pushStart;
	- m_pushEnd;
- DocumentLoadTiming
	- m_referenceMonotonicTime;
	- m_referenceWallTime;
	- m_navigationStart;
	- m_unloadEventStart;
	- m_unloadEventEnd;
	- m_redirectStart;
	- m_redirectEnd;
	- m_redirectCount;
	- m_fetchStart;
	- m_responseEnd; //没有responseStart
	- m_loadEventStart; //Document::implicitClose时调用，在layout之前
	- m_loadEventEnd;
	- m_hasCrossOriginRedirect;
	- m_hasSameOriginAsPreviousDocument;
- DocumentParserTiming //parser相关的timing，不在spec中
	- m_parserStart
	- m_parserStop
	- m_parserBlockedOnScriptLoadDuration
	- m_parserBlockedOnScriptLoadFromDocumentWriteDuration
	- m_parserBlockedOnScriptExecutionDuration
	- m_parserBlockedOnScriptExecutionFromDocumentWriteDuration 
- documentTiming:
	- m_domLoading
	- m_domInteractive
	- m_domContentLoadedEventStart
	- m_domContentLoadedEventEnd
	- m_domComplete
	- m_firstLayout
- CSSTiming
	- m_parseTimeBeforeFCP = 0;
	- m_updateTimeBeforeFCP = 0;
	- m_paintTiming
		- m_firstPaint = 0.0;
		- m_firstTextPaint = 0.0;
		- m_firstImagePaint = 0.0;
		- m_firstContentfulPaint = 0.0;
		- m_firstMeaningfulPaint = 0.0;
		- m_firstMeaningfulPaintCandidate = 0.0;
		- m_fmpDetector // first meaningful paint

