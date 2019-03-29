# IDL

IDLAttributes.json定义了IDL的attrbutes
- ImplementationLacksVTable：如果没有virtual，要包含这个。
- ActiveDOMObject: document释放前会调用stop

```
#0  WebCore::ResizeObserver::stop (this=0x7f0d924ca880) at ../../Source/WebCore/page/ResizeObserver.cpp:92
#1  0x00007f0e1c6c2b59 in WebCore::ScriptExecutionContext::<lambda(auto:19&)>::operator()<WebCore::ActiveDOMObject>(WebCore::ActiveDOMObject &) const (__closure=0x7f0d924d1588, activeDOMObject=...)
    at ../../Source/WebCore/dom/ScriptExecutionContext.cpp:320
#2  0x00007f0e1c6c2b90 in WTF::Function<WebCore::ScriptExecutionContext::ShouldContinue(WebCore::ActiveDOMObject&)>::CallableWrapper<WebCore::ScriptExecutionContext::stopActiveDOMObjects()::<lambda(auto:19&)> >::call(WebCore::ActiveDOMObject &) (this=0x7f0d924d1580, in#0=...) at DerivedSources/ForwardingHeaders/wtf/Function.h:101
#3  0x00007f0e1c6c5534 in WTF::Function<WebCore::ScriptExecutionContext::ShouldContinue (WebCore::ActiveDOMObject&)>::operator()(WebCore::ActiveDOMObject&) const (this=0x7fff6c045850, in#0=...)
    at DerivedSources/ForwardingHeaders/wtf/Function.h:56
#4  0x00007f0e1c6bab02 in WebCore::ScriptExecutionContext::forEachActiveDOMObject(WTF::Function<WebCore::ScriptExecutionContext::ShouldContinue (WebCore::ActiveDOMObject&)> const&) const (
    this=0x7f0d91801058, apply=...) at ../../Source/WebCore/dom/ScriptExecutionContext.cpp:270
#5  0x00007f0e1c6bad1f in WebCore::ScriptExecutionContext::stopActiveDOMObjects (this=0x7f0d91801058) at ../../Source/WebCore/dom/ScriptExecutionContext.cpp:319
#6  0x00007f0e1c563fbe in WebCore::Document::stopActiveDOMObjects (this=0x7f0d91800fb8) at ../../Source/WebCore/dom/Document.cpp:2676
#7  0x00007f0e1c563ae3 in WebCore::Document::prepareForDestruction (this=0x7f0d91800fb8) at ../../Source/WebCore/dom/Document.cpp:2533
#8  0x00007f0e1cd84703 in WebCore::Frame::setView (this=0x7f0dfa399230, view=...) at ../../Source/WebCore/page/Frame.cpp:240
#9  0x00007f0e1cd86c69 in WebCore::Frame::createView (this=0x7f0dfa399230, viewportSize=..., transparent=false, fixedLayoutSize=..., fixedVisibleContentRect=..., useFixedLayout=false, 
    horizontalScrollbarMode=WebCore::ScrollbarAuto, horizontalLock=false, verticalScrollbarMode=WebCore::ScrollbarAuto, verticalLock=false) at ../../Source/WebCore/page/Frame.cpp:917
#10 0x00007f0e1b109a67 in WebKit::WebFrameLoaderClient::transitionToCommittedForNewPage (this=0x55f406a5f2c0) at ../../Source/WebKit/WebProcess/WebCoreSupport/WebFrameLoaderClient.cpp:1421
#11 0x00007f0e1cc0579f in WebCore::FrameLoader::transitionToCommitted (this=0x55f4069f8650, cachedPage=0x0) at ../../Source/WebCore/loader/FrameLoader.cpp:2175
#12 0x00007f0e1cc04ae5 in WebCore::FrameLoader::commitProvisionalLoad (this=0x55f4069f8650) at ../../Source/WebCore/loader/FrameLoader.cpp:2002
#13 0x00007f0e1cbcfad3 in WebCore::DocumentLoader::commitIfReady (this=0x7f0d9246b000) at ../../Source/WebCore/loader/DocumentLoader.cpp:365
#14 0x00007f0e1cbd2f9b in WebCore::DocumentLoader::commitLoad (this=0x7f0d9246b000, 

```
