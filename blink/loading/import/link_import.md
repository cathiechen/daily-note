#link import
`<link rel="import" href="xxx">`

[issue](https://bugs.chromium.org/p/chromium/issues/detail?id=629236&q=label%3AHotlist-GoodFirstBug%20component%3ABlink%3ELayout&colspec=ID%20Pri%20M%20Stars%20ReleaseBlock%20Component%20Status%20Owner%20Summary%20OS%20Modified)

[引入的修改](https://chromium.googlesource.com/chromium/src/+/6e22c58ca54e6b5d6584fdb21818bf78973ed893%5E%21/third_party/WebKit/Source/core/css/resolver/StyleResolver.cpp)

##过程
重现问题：

	<div>hello</div>
	<script>
	  var link = document.createElement('link');
	  link.rel = 'import';
	  link.href = 'bad/import.html';
	  document.head.appendChild(link);    
	  var div = document.querySelector('div');
	  console.log(div.getBoundingClientRect());
	</script>

document.h:

	  bool isRenderingReady() const {
	    return haveImportsLoaded() && haveRenderBlockingStylesheetsLoaded();
	  }

- 创建link，并`appendChild`，这个过程会造成`isRenderingReady`为false。因为它有一个unloaded的import
- `getBoundingClientRect`的流程是这样的。先判断element是否需要updatesyle，若需要，updatesytle；是否需要构建layoutobject，yes，构建；是否需要layout，yes，layout。

		blink::StyleResolver::styleForElement (this=0x81199360, element=0x2c217800, defaultParent=0x0, defaultLayoutParent=<optimized out>, sharingBehavior=blink::AllowStyleSharing,
		matchingBehavior=blink::MatchAllRules) at ../../third_party/WebKit/Source/core/css/resolver/StyleResolver.cpp:645
		0x7ada6bc2 in blink::Document::inheritHtmlAndBodyElementStyles (this=0x2c216e40, change=<optimized out>) at ../../third_party/WebKit/Source/core/dom/Document.cpp:1776
		0x7ada78bc in blink::Document::updateStyle (this=0x2c216e40) at ../../third_party/WebKit/Source/core/dom/Document.cpp:2098
		0x7ada53ba in blink::Document::updateStyleAndLayoutTree (this=0x2c216e40) at ../../third_party/WebKit/Source/core/dom/Document.cpp:2019
		0x7ada85b4 in blink::Document::updateStyleAndLayoutTreeIgnorePendingStylesheets (this=0x2c216e40) at ../../third_party/WebKit/Source/core/dom/Document.cpp:2290
		0x7ada832c in blink::Document::updateStyleAndLayoutIgnorePendingStylesheets (this=0x2c216e40, runPostLayoutTasks=blink::Document::RunPostLayoutTasksAsyhnchronously)
		at ../../third_party/WebKit/Source/core/dom/Document.cpp:2295
		blink::Document::updateStyleAndLayoutIgnorePendingStylesheetsForNode (this=0x2c216e40, node=0x2c2178b0) at ../../third_party/WebKit/Source/core/dom/Document.cpp:2185
		0x7add45e4 in blink::Element::clientQuads (this=0x2c2178b0, quads=WTF::Vector of length 0, capacity 0) at ../../third_party/WebKit/Source/core/dom/Element.cpp:1128
		0x7add47b8 in blink::Element::getBoundingClientRect (this=0x2c2178b0) at ../../third_party/WebKit/Source/core/dom/Element.cpp:1162
		0x7b4abc38 in blink::ElementV8Internal::getBoundingClientRectMethod (info=...) at gen/blink/bindings/core/v8/V8Element.cpp:1339
- 整个流程在`styleForElement`中被block了。原因是它有个import，没下完。
- 这个bug只会发生在js动态创建import，并加入dom树中。原因是js动态创建，他们没法block script，所以才会继续执行js，才会调用到`getBoundingClientRect`，才有后面的事情。

##疑问
`import`是否应该阻塞render？如果你允许js继续执行`getBoundingClientRect`，那你就不该阻塞render.

##关于`import`
- [标准](http://w3c.github.io/webcomponents/spec/imports/#dfn-import-async-attribute)
- [如何使用](https://www.html5rocks.com/en/tutorials/webcomponents/imports/)
- 作为[web compornent](https://developer.mozilla.org/en-US/docs/Web/Web_Components)中打包资源用的。
- 实现，[详见](https://cs.chromium.org/chromium/src/third_party/WebKit/Source/core/html/imports/HTMLImport.h)
	- 数据结构![](http://i.imgur.com/66uIPZT.jpg)
- 如何与document.isRenderingReady联系起来的？htmlimportstatus