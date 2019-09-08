#Scroll process in WebKit

##Scroll triggered by UI process. From UI Process To Web Process, then back to UI Process

### UI process:

```
#0  0x0000000114dae647 in WebKit::RemoteScrollingCoordinatorProxy::scrollingTreeNodeDidScroll(unsigned long long, WebCore::FloatPoint const&, WTF::Optional<WebCore::FloatPoint> const&, WebCore::ScrollingLayerPositionAction) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/UIProcess/RemoteLayerTree/RemoteScrollingCoordinatorProxy.cpp:224
#1  0x0000000114daeba6 in WebKit::RemoteScrollingTree::scrollingTreeNodeDidScroll(WebCore::ScrollingTreeScrollingNode&, WebCore::ScrollingLayerPositionAction) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/UIProcess/RemoteLayerTree/RemoteScrollingTree.cpp:107
#2  0x00000001294dde0f in WebCore::ScrollingTreeScrollingNode::wasScrolledByDelegatedScrolling(WebCore::FloatPoint const&, WTF::Optional<WebCore::FloatRect>, WebCore::ScrollingLayerPositionAction) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/ScrollingTreeScrollingNode.cpp:209
#3  0x0000000114b47d7e in WebKit::ScrollingTreeScrollingNodeDelegateIOS::scrollViewDidScroll(WebCore::FloatPoint const&, bool) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/UIProcess/RemoteLayerTree/ios/ScrollingTreeScrollingNodeDelegateIOS.mm:335
#4  0x0000000114b47cc2 in ::-[WKScrollingNodeScrollViewDelegate scrollViewDidScroll:](UIScrollView *) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/UIProcess/RemoteLayerTree/ios/ScrollingTreeScrollingNodeDelegateIOS.mm:73
#5  0x0000000121269ff3 in -[UIScrollView(UIScrollViewInternal) _notifyDidScroll] ()
```

In `RemoteScrollingCoordinatorProxy::scrollingTreeNodeDidScroll` send message to web process by `m_webPageProxy.send(Messages::RemoteScrollingCoordinator::ScrollPositionChangedForNode)`. 

### In Web Process:

```
#0  0x0000000516943545 in WebCore::AsyncScrollingCoordinator::scheduleUpdateScrollPositionAfterAsyncScroll(unsigned long long, WebCore::FloatPoint const&, WTF::Optional<WebCore::FloatPoint> const&, WebCore::ScrollingLayerPositionAction) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/AsyncScrollingCoordinator.cpp:291
#1  0x0000000107c51c5f in WebKit::RemoteScrollingCoordinator::scrollPositionChangedForNode(unsigned long long, WebCore::FloatPoint const&, bool) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/WebProcess/WebPage/RemoteLayerTree/RemoteScrollingCoordinator.mm:99
```

Start a timer m_updateNodeScrollPositionTimer. When it fired, or the scroll action matches.

```
enum class ScrollingLayerPositionAction {
    Set,
    SetApproximate,
    Sync
};
```

Then:

```
#0  0x0000000517052160 in WebCore::RenderLayer::scrollTo(WebCore::IntPoint const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayer.cpp:2540
#1  0x0000000517055a2d in WebCore::RenderLayer::setScrollOffset(WebCore::IntPoint const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayer.cpp:3014
#2  0x00000005169cfd3e in WebCore::ScrollableArea::scrollPositionChanged(WebCore::IntPoint const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/ScrollableArea.cpp:181
#3  0x00000005169c6049 in WebCore::ScrollableArea::setScrollOffsetFromAnimation(WebCore::IntPoint const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/ScrollableArea.cpp:248
#4  0x00000005169c5f2e in WebCore::ScrollAnimator::notifyPositionChanged(WebCore::FloatSize const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/ScrollAnimator.cpp:223
#5  0x00000005169c5819 in WebCore::ScrollAnimator::scrollToOffsetWithoutAnimation(WebCore::FloatPoint const&, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/ScrollAnimator.cpp:112
#6  0x00000005169cb1d9 in WebCore::ScrollableArea::scrollToOffsetWithoutAnimation(WebCore::FloatPoint const&, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/ScrollableArea.cpp:159
#7  0x000000051694314b in WebCore::AsyncScrollingCoordinator::updateScrollPositionAfterAsyncScroll(unsigned long long, WebCore::FloatPoint const&, WTF::Optional<WebCore::FloatPoint>, WebCore::ScrollType, WebCore::ScrollingLayerPositionAction) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/AsyncScrollingCoordinator.cpp:364
#8  0x00000005169434da in WebCore::AsyncScrollingCoordinator::scheduleUpdateScrollPositionAfterAsyncScroll(unsigned long long, WebCore::FloatPoint const&, WTF::Optional<WebCore::FloatPoint> const&, WebCore::ScrollingLayerPositionAction) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/AsyncScrollingCoordinator.cpp:285

```

So the action from UI scroll view to web core is called sync. RenderLayer::scrollTo changes the m_scrollPosition in RenderLayer.


usesCompositedScrolling() determined if the renderer should repaint or not.

```
if (usesCompositedScrolling())
    setNeedsCompositingGeometryUpdate();
```

The prcess:

```
RenderLayerCompositor:
    RenderLayerCompositor::updateBackingAndHierarchy
        => update RenderLayer layer flags, e.g. needsCompositingGeometryUpdate
        => layerBacking->updateGeometry();
        => layerBacking->updateEventRegion();
        => deal with child layers.

Then RenderLayerCompositor will call RenderLayerBacking's updateGeometry: change the origin of GraphicsLayer and others scroll offset and size.

```

```
#0  0x0000000516b03e20 in WebCore::GraphicsLayer::syncBoundsOrigin(WebCore::FloatPoint const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/graphics/GraphicsLayer.h:349
#0  0x0000000516b7ed97 in WebCore::GraphicsLayerCA::syncBoundsOrigin(WebCore::FloatPoint const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/graphics/ca/GraphicsLayerCA.cpp:652
#1  0x000000051707a825 in WebCore::RenderLayerBacking::setLocationOfScrolledContents(WebCore::IntPoint, WebCore::ScrollingLayerPositionAction) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayerBacking.cpp:1384
#2  0x000000051707a5c3 in WebCore::RenderLayerBacking::updateScrollOffset(WebCore::IntPoint) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayerBacking.cpp:1393
#3  0x00000005170792d7 in WebCore::RenderLayerBacking::updateGeometry() at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayerBacking.cpp:1304
#4  0x00000005170a72ab in WebCore::RenderLayerCompositor::updateBackingAndHierarchy(WebCore::RenderLayer&, WTF::Vector<WTF::Ref<WebCore::GraphicsLayer, WTF::DumbPtrTraits<WebCore::GraphicsLayer> >, 0ul, WTF::CrashOnOverflow, 16ul>&, WebCore::ScrollingTreeState&, WTF::OptionSet<WebCore::RenderLayerCompositor::UpdateLevel>, int) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayerCompositor.cpp:1220
#5  0x00000005170a7763 in WebCore::RenderLayerCompositor::updateBackingAndHierarchy(WebCore::RenderLayer&, WTF::Vector<WTF::Ref<WebCore::GraphicsLayer, WTF::DumbPtrTraits<WebCore::GraphicsLayer> >, 0ul, WTF::CrashOnOverflow, 16ul>&, WebCore::ScrollingTreeState&, WTF::OptionSet<WebCore::RenderLayerCompositor::UpdateLevel>, int) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayerCompositor.cpp:1285
#6  0x00000005170a7827 in WebCore::RenderLayerCompositor::updateBackingAndHierarchy(WebCore::RenderLayer&, WTF::Vector<WTF::Ref<WebCore::GraphicsLayer, WTF::DumbPtrTraits<WebCore::GraphicsLayer> >, 0ul, WTF::CrashOnOverflow, 16ul>&, WebCore::ScrollingTreeState&, WTF::OptionSet<WebCore::RenderLayerCompositor::UpdateLevel>, int) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayerCompositor.cpp:1288
#7  0x00000005170a3c29 in WebCore::RenderLayerCompositor::updateCompositingLayers(WebCore::CompositingUpdateType, WebCore::RenderLayer*) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayerCompositor.cpp:798
#8  0x0000000517052dc0 in WebCore::RenderLayer::updateCompositingLayersAfterScroll() at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayer.cpp:2820
#9  0x000000051705246f in WebCore::RenderLayer::scrollTo(WebCore::IntPoint const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayer.cpp:2600


```

The process:

```
RenderLayerBacking::updateGeometry
    update size of m_graphicsLayer,
    RenderLayerBacking::updateScrollOffset, actually change the bounds origin of m_scrollContainerLayer by m_scrollContainerLayer->syncBoundsOrigin
    update the position, size and scrollOffset of m_scrollContainerLayer and m_scrolledContentsLayer.
    setRequiresOwnBackingStore
        => m_owningLayer.computeRepaintRectsIncludingDescendants(); 
        => compositor().repaintInCompositedAncestor
```

Then call layerBacking->updateEventRegion():

```
RenderLayerBacking::updateEventRegion
    => m_owningLayer.paintLayerContents
    => eventRegion.translate(layerOffset);
    => layerForEventRegion.setEventRegion(WTFMove(eventRegion));
        => change the origin of graphicsLayer and schedule flush.
            => RenderLayerBacking::notifyFlushRequired
                => compositor().scheduleLayerFlush
                => RenderLayerCompositor::scheduleLayerFlush
                => page().renderingUpdateScheduler().scheduleRenderingUpdate()
```

This will schedule flush, see:

```

#0  0x0000000106ff8511 in WebKit::RemoteLayerTreeDrawingArea::scheduleCompositingLayerFlush() at /Users/cathiechen/cc/source/WebKit/Source/WebKit/WebProcess/WebPage/RemoteLayerTree/RemoteLayerTreeDrawingArea.mm:301
#1  0x0000000107c5137d in WebKit::RemoteLayerTreeDisplayRefreshMonitor::requestRefreshCallback() at /Users/cathiechen/cc/source/WebKit/Source/WebKit/WebProcess/WebPage/RemoteLayerTree/RemoteLayerTreeDisplayRefreshMonitor.mm:52
#2  0x0000000516a91332 in WebCore::DisplayRefreshMonitorManager::scheduleAnimation(WebCore::DisplayRefreshMonitorClient&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/graphics/DisplayRefreshMonitorManager.cpp:98
#3  0x0000000516868f6e in WebCore::RenderingUpdateScheduler::scheduleTimedRenderingUpdate() at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/RenderingUpdateScheduler.cpp:59
#4  0x00000005168694a0 in WebCore::RenderingUpdateScheduler::scheduleRenderingUpdate() at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/RenderingUpdateScheduler.cpp:118
#5  0x00000005170a4109 in WebCore::RenderLayerCompositor::scheduleLayerFlush(bool) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayerCompositor.cpp:475
#6  0x0000000517080c45 in WebCore::RenderLayerBacking::notifyFlushRequired(WebCore::GraphicsLayer const*) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayerBacking.cpp:3336
#7  0x0000000516b7d657 in WebCore::GraphicsLayerCA::noteLayerPropertyChanged(unsigned long long, WebCore::GraphicsLayerCA::ScheduleFlushOrNot) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/graphics/ca/GraphicsLayerCA.cpp:4217
#8  0x0000000516b80689 in WebCore::GraphicsLayerCA::setEventRegion(WebCore::EventRegion&&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/graphics/ca/GraphicsLayerCA.cpp:999
#9  0x000000051707bdaf in WebCore::RenderLayerBacking::updateEventRegion() at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayerBacking.cpp:1613
#10 0x00000005170a733d in WebCore::RenderLayerCompositor::updateBackingAndHierarchy(WebCore::RenderLayer&, WTF::Vector<WTF::Ref<WebCore::GraphicsLayer, WTF::DumbPtrTraits<WebCore::GraphicsLayer> >, 0ul, WTF::CrashOnOverflow, 16ul>&, WebCore::ScrollingTreeState&, WTF::OptionSet<WebCore::RenderLayerCompositor::UpdateLevel>, int) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayerCompositor.cpp:1228


```
If the timer fired, it will call `RemoteLayerTreeDrawingArea::flushLayers` which will send `RemoteLayerTreeDrawingAreaProxy::CommitLayerTree` to UI process. RemoteScrollingCoordinatorTransaction will contain 
ScrollingTree, ThreadedScrollingTree, have ScrollingTreeScrollingNode which might contains scroll info ScrollingStateScrollingNode::RequestedScrollPosition that make UI scroll view scrolling. But in this case, it won't contain, scrollingCoordinator->requestScrollPositionUpdate will.

RemoteScrollingCoordinatorTransaction::encode will encode its rootNode i.e. ScrollingStateFrameScrollingNode and its children node, e.g. ScrollingStateOverflowScrollingNode which might set ScrollingStateScrollingNode::RequestedScrollPosition, but not in this case.


### Back to UI Process:
Through IPC, RemoteScrollingCoordinatorTransaction::decode is called in UI porcess which will get info from RemoteScrollingCoordinatorTransaction.

```
// Not important, just in case you are interested in ipc connection.

#3  0x00000001294c676f in WebCore::ScrollingStateTree::insertNode(WebCore::ScrollingNodeType, unsigned long long, unsigned long long, unsigned long) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/ScrollingStateTree.cpp:167
#4  0x0000000114815e90 in WebKit::RemoteScrollingCoordinatorTransaction::decode(IPC::Decoder&) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/Shared/RemoteLayerTree/RemoteScrollingCoordinatorTransaction.cpp:546
#5  0x0000000114815d2d in WebKit::RemoteScrollingCoordinatorTransaction::decode(IPC::Decoder&, WebKit::RemoteScrollingCoordinatorTransaction&) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/Shared/RemoteLayerTree/RemoteScrollingCoordinatorTransaction.cpp:512
#6  0x000000011423d28d in bool IPC::ArgumentCoder<WebKit::RemoteScrollingCoordinatorTransaction>::decode<WebKit::RemoteScrollingCoordinatorTransaction, (void*)0>(IPC::Decoder&, WebKit::RemoteScrollingCoordinatorTransaction&) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/Platform/IPC/ArgumentCoder.h:105
#7  0x000000011423cfee in IPC::Decoder& IPC::Decoder::operator>><WebKit::RemoteScrollingCoordinatorTransaction, (void*)0>(WTF::Optional<WebKit::RemoteScrollingCoordinatorTransaction>&) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/Platform/IPC/Decoder.h:176
#8  0x000000011423ca00 in IPC::TupleDecoderImpl<WebKit::RemoteScrollingCoordinatorTransaction>::decode(IPC::Decoder&) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/Platform/IPC/ArgumentCoders.h:213
#9  0x000000011423c7d5 in IPC::TupleDecoderImpl<WebKit::RemoteLayerTreeTransaction, WebKit::RemoteScrollingCoordinatorTransaction>::decode(IPC::Decoder&) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/Platform/IPC/ArgumentCoders.h:200
#10 0x000000011423c73c in IPC::TupleDecoder<2ul, WebKit::RemoteLayerTreeTransaction, WebKit::RemoteScrollingCoordinatorTransaction>::decode(IPC::Decoder&) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/Platform/IPC/ArgumentCoders.h:224
#11 0x000000011423c5dc in IPC::ArgumentCoder<std::__1::tuple<WebKit::RemoteLayerTreeTransaction, WebKit::RemoteScrollingCoordinatorTransaction> >::decode(IPC::Decoder&) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/Platform/IPC/ArgumentCoders.h:244
#12 0x000000011423c3aa in IPC::Decoder& IPC::Decoder::operator>><std::__1::tuple<WebKit::RemoteLayerTreeTransaction, WebKit::RemoteScrollingCoordinatorTransaction>, (void*)0>(WTF::Optional<std::__1::tuple<WebKit::RemoteLayerTreeTransaction, WebKit::RemoteScrollingCoordinatorTransaction> >&) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/Platform/IPC/Decoder.h:168
#13 0x000000011423b0b7 in void IPC::handleMessage<Messages::RemoteLayerTreeDrawingAreaProxy::CommitLayerTree, WebKit::RemoteLayerTreeDrawingAreaProxy, void (WebKit::RemoteLayerTreeDrawingAreaProxy::*)(WebKit::RemoteLayerTreeTransaction const&, WebKit::RemoteScrollingCoordinatorTransaction const&)>(IPC::Decoder&, WebKit::RemoteLayerTreeDrawingAreaProxy*, void (WebKit::RemoteLayerTreeDrawingAreaProxy::*)(WebKit::RemoteLayerTreeTransaction const&, WebKit::RemoteScrollingCoordinatorTransaction const&)) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/Platform/IPC/HandleMessage.h:114
#14 0x000000011423af44 in WebKit::RemoteLayerTreeDrawingAreaProxy::didReceiveMessage(IPC::Connection&, IPC::Decoder&) at /Users/cathiechen/cc/source/WebKit/WebKitBuild/Debug-iphonesimulator/DerivedSources/WebKit2/RemoteLayerTreeDrawingAreaProxyMessageReceiver.cpp:45
#15 0x00000001140ce559 in IPC::MessageReceiverMap::dispatchMessage(IPC::Connection&, IPC::Decoder&) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/Platform/IPC/MessageReceiverMap.cpp:123
#16 0x000000011484cbc4 in WebKit::AuxiliaryProcessProxy::dispatchMessage(IPC::Connection&, IPC::Decoder&) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/UIProcess/AuxiliaryProcessProxy.cpp:155
#17 0x0000000114ac682a in WebKit::WebProcessProxy::didReceiveMessage(IPC::Connection&, IPC::Decoder&) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/UIProcess/WebProcessProxy.cpp:662
#18 0x0000000114066099 in IPC::Connection::dispatchMessage(IPC::Decoder&) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/Platform/IPC/Connection.cpp:939
#19 0x000000011405f9fb in IPC::Connection::dispatchMessage(std::__1::unique_ptr<IPC::Decoder, std::__1::default_delete<IPC::Decoder> >) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/Platform/IPC/Connection.cpp:991
#20 0x000000011406517b in IPC::Connection::dispatchIncomingMessages() at /Users/cathiechen/cc/source/WebKit/Source/WebKit/Platform/IPC/Connection.cpp:1095
#21 0x0000000114080925 in IPC::Connection::enqueueIncomingMessage(std::__1::unique_ptr<IPC::Decoder, std::__1::default_delete<IPC::Decoder> >)::$_7::operator()() at /Users/cathiechen/cc/source/WebKit/Source/WebKit/Platform/IPC/Connection.cpp:914

```

Then called `RemoteLayerTreeDrawingAreaProxy::commitLayerTree`, but won't change scrolloffset of UI scroll view in this case.


-------------
## ScrollOffset snyc from Web Process To UI process: ProgrammaticScroll, e.g. getElementById(x).scrollLeft = xx;

### Web Process
```
#0  0x0000000517051861 in WebCore::RenderLayer::scrollToOffset(WebCore::IntPoint const&, WebCore::ScrollType, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayer.cpp:2498
#1  0x0000000517051c25 in WebCore::RenderLayer::scrollToYPosition(int, WebCore::ScrollType, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayer.cpp:2478
#2  0x0000000516f3bc1b in WebCore::RenderBox::setScrollTop(int, WebCore::ScrollType, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderBox.cpp:591
#3  0x0000000515b8a44d in WebCore::Element::setScrollTop(int) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/dom/Element.cpp:1270
#4  0x0000000514074a0a in WebCore::setJSElementScrollTopSetter(JSC::ExecState&, WebCore::JSElement&, JSC::JSValue, JSC::ThrowScope&)::'lambda'()::operator()() const at /Users/cathiechen/cc/source/WebKit/WebKitBuild/Debug-iphonesimulator/DerivedSources/WebCore/JSElement.cpp:1281


```

In RenderLayer::scrollToOffset, it uses:

```
    if (ScrollingCoordinator* scrollingCoordinator = page().scrollingCoordinator())
        handled = scrollingCoordinator->requestScrollPositionUpdate(*this, scrollPositionFromOffset(clampedScrollOffset));
```
Not scrollToOffsetWithoutAnimation to scroll layer.

So what does requestScrollPositionUpdate do?

```
#3  0x000000051694314b in WebCore::AsyncScrollingCoordinator::updateScrollPositionAfterAsyncScroll(unsigned long long, WebCore::FloatPoint const&, WTF::Optional<WebCore::FloatPoint>, WebCore::ScrollType, WebCore::ScrollingLayerPositionAction) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/AsyncScrollingCoordinator.cpp:364
#4  0x0000000516942aa4 in WebCore::AsyncScrollingCoordinator::requestScrollPositionUpdate(WebCore::ScrollableArea&, WebCore::IntPoint const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/AsyncScrollingCoordinator.cpp:252

```


also, it will also update the scroll offset in scrollablearea

```
#0  0x00000005169c5e30 in WebCore::ScrollAnimator::notifyPositionChanged(WebCore::FloatSize const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/ScrollAnimator.cpp:222
#1  0x00000005169c5819 in WebCore::ScrollAnimator::scrollToOffsetWithoutAnimation(WebCore::FloatPoint const&, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/ScrollAnimator.cpp:112
#2  0x00000005169cb1d9 in WebCore::ScrollableArea::scrollToOffsetWithoutAnimation(WebCore::FloatPoint const&, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/ScrollableArea.cpp:159
#3  0x000000051694314b in WebCore::AsyncScrollingCoordinator::updateScrollPositionAfterAsyncScroll(unsigned long long, WebCore::FloatPoint const&, WTF::Optional<WebCore::FloatPoint>, WebCore::ScrollType, WebCore::ScrollingLayerPositionAction) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/AsyncScrollingCoordinator.cpp:364
#4  0x0000000516942aa4 in WebCore::AsyncScrollingCoordinator::requestScrollPositionUpdate(WebCore::ScrollableArea&, WebCore::IntPoint const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/AsyncScrollingCoordinator.cpp:252
#5  0x00000005170518a2 in WebCore::RenderLayer::scrollToOffset(WebCore::IntPoint const&, WebCore::ScrollType, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayer.cpp:2498
#6  0x0000000517051c25 in WebCore::RenderLayer::scrollToYPosition(int, WebCore::ScrollType, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayer.cpp:2478
#7  0x0000000516f3bc1b in WebCore::RenderBox::setScrollTop(int, WebCore::ScrollType, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderBox.cpp:591
#8  0x0000000515b8a44d in WebCore::Element::setScrollTop(int) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/dom/Element.cpp:1270


```

But then, trigger the m_layerFlushTimer in this path:


```
// TODO: Need to look into the details.

#0  0x0000000106ff8384 in WebKit::RemoteLayerTreeDrawingArea::scheduleCompositingLayerFlushImmediately() at /Users/cathiechen/cc/source/WebKit/Source/WebKit/WebProcess/WebPage/RemoteLayerTree/RemoteLayerTreeDrawingArea.mm:271
#1  0x0000000106ff84d0 in WebKit::RemoteLayerTreeDrawingArea::scheduleCompositingLayerFlush() at /Users/cathiechen/cc/source/WebKit/Source/WebKit/WebProcess/WebPage/RemoteLayerTree/RemoteLayerTreeDrawingArea.mm:293
#2  0x0000000107c51a00 in WebKit::RemoteScrollingCoordinator::scheduleTreeStateCommit() at /Users/cathiechen/cc/source/WebKit/Source/WebKit/WebProcess/WebPage/RemoteLayerTree/RemoteScrollingCoordinator.mm:64
#3  0x0000000516941ab9 in WebCore::AsyncScrollingCoordinator::scrollingStateTreePropertiesChanged() at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/AsyncScrollingCoordinator.cpp:70
#4  0x000000051696e7fc in WebCore::ScrollingStateTree::setHasChangedProperties(bool) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/ScrollingStateTree.cpp:62
#5  0x000000051696206f in WebCore::ScrollingStateNode::setPropertyChanged(unsigned int) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/ScrollingStateNode.cpp:68
#6  0x000000051696be0f in WebCore::ScrollingStateScrollingNode::setScrollPosition(WebCore::FloatPoint const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/ScrollingStateScrollingNode.cpp:145
#7  0x00000005169457eb in WebCore::AsyncScrollingCoordinator::setScrollingNodeScrollableAreaGeometry(unsigned long long, WebCore::ScrollableArea&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/AsyncScrollingCoordinator.cpp:688
#8  0x00000005170b5af8 in WebCore::RenderLayerCompositor::updateScrollingNodeForScrollingRole(WebCore::RenderLayer&, WebCore::ScrollingTreeState&, WTF::OptionSet<WebCore::RenderLayerCompositor::ScrollingNodeChangeFlags>) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayerCompositor.cpp:4529
#9  0x00000005170aa8fc in WebCore::RenderLayerCompositor::updateScrollCoordinationForLayer(WebCore::RenderLayer&, WebCore::ScrollingTreeState&, WTF::OptionSet<WebCore::RenderLayerCompositor::ScrollingNodeChangeFlags>) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayerCompositor.cpp:4400
#10 0x00000005170a73f0 in WebCore::RenderLayerCompositor::updateBackingAndHierarchy(WebCore::RenderLayer&, WTF::Vector<WTF::Ref<WebCore::GraphicsLayer, WTF::DumbPtrTraits<WebCore::GraphicsLayer> >, 0ul, WTF::CrashOnOverflow, 16ul>&, WebCore::ScrollingTreeState&, WTF::OptionSet<WebCore::RenderLayerCompositor::UpdateLevel>, int) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayerCompositor.cpp:1243
#11 0x00000005170a7763 in WebCore::RenderLayerCompositor::updateBackingAndHierarchy(WebCore::RenderLayer&, WTF::Vector<WTF::Ref<WebCore::GraphicsLayer, WTF::DumbPtrTraits<WebCore::GraphicsLayer> >, 0ul, WTF::CrashOnOverflow, 16ul>&, WebCore::ScrollingTreeState&, WTF::OptionSet<WebCore::RenderLayerCompositor::UpdateLevel>, int) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayerCompositor.cpp:1285
#12 0x00000005170a7827 in WebCore::RenderLayerCompositor::updateBackingAndHierarchy(WebCore::RenderLayer&, WTF::Vector<WTF::Ref<WebCore::GraphicsLayer, WTF::DumbPtrTraits<WebCore::GraphicsLayer> >, 0ul, WTF::CrashOnOverflow, 16ul>&, WebCore::ScrollingTreeState&, WTF::OptionSet<WebCore::RenderLayerCompositor::UpdateLevel>, int) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayerCompositor.cpp:1288
#13 0x00000005170a3c29 in WebCore::RenderLayerCompositor::updateCompositingLayers(WebCore::CompositingUpdateType, WebCore::RenderLayer*) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayerCompositor.cpp:798
#14 0x0000000517052dc0 in WebCore::RenderLayer::updateCompositingLayersAfterScroll() at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayer.cpp:2820
#15 0x000000051705246f in WebCore::RenderLayer::scrollTo(WebCore::IntPoint const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayer.cpp:2600
#16 0x0000000517055a2d in WebCore::RenderLayer::setScrollOffset(WebCore::IntPoint const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayer.cpp:3014
#17 0x00000005169cfd3e in WebCore::ScrollableArea::scrollPositionChanged(WebCore::IntPoint const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/ScrollableArea.cpp:181
#18 0x00000005169c6049 in WebCore::ScrollableArea::setScrollOffsetFromAnimation(WebCore::IntPoint const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/ScrollableArea.cpp:248
#19 0x00000005169c5f2e in WebCore::ScrollAnimator::notifyPositionChanged(WebCore::FloatSize const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/ScrollAnimator.cpp:223
#20 0x00000005169c5819 in WebCore::ScrollAnimator::scrollToOffsetWithoutAnimation(WebCore::FloatPoint const&, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/ScrollAnimator.cpp:112
#21 0x00000005169cb1d9 in WebCore::ScrollableArea::scrollToOffsetWithoutAnimation(WebCore::FloatPoint const&, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/ScrollableArea.cpp:159
#22 0x000000051694314b in WebCore::AsyncScrollingCoordinator::updateScrollPositionAfterAsyncScroll(unsigned long long, WebCore::FloatPoint const&, WTF::Optional<WebCore::FloatPoint>, WebCore::ScrollType, WebCore::ScrollingLayerPositionAction) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/AsyncScrollingCoordinator.cpp:364
#23 0x0000000516942aa4 in WebCore::AsyncScrollingCoordinator::requestScrollPositionUpdate(WebCore::ScrollableArea&, WebCore::IntPoint const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/AsyncScrollingCoordinator.cpp:252
#24 0x00000005170518a2 in WebCore::RenderLayer::scrollToOffset(WebCore::IntPoint const&, WebCore::ScrollType, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayer.cpp:2498
#25 0x0000000517051c25 in WebCore::RenderLayer::scrollToYPosition(int, WebCore::ScrollType, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayer.cpp:2478
#26 0x0000000516f3bc1b in WebCore::RenderBox::setScrollTop(int, WebCore::ScrollType, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderBox.cpp:591
#27 0x0000000515b8a44d in WebCore::Element::setScrollTop(int) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/dom/Element.cpp:1270
#28 0x0000000514074a0a in WebCore::setJSElementScrollTopSetter(JSC::ExecState&, WebCore::JSElement&, JSC::JSValue, JSC::ThrowScope&)::'lambda'()::operator()() const at /Users/cathiechen/cc/source/WebKit/WebKitBuild/Debug-iphonesimulator/DerivedSources/WebCore/JSElement.cpp:1281


```
`ScrollingStateScrollingNode::setScrollPosition` will update the scroll position and properties. then scheduleCompositingLayerFlush.
ScrollingStateScrollingNode::setRequestedScrollPosition will be set.

```
#0  0x000000051696c5eb in WebCore::ScrollingStateScrollingNode::setRequestedScrollPosition(WebCore::FloatPoint const&, bool) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/ScrollingStateScrollingNode.cpp:224
#1  0x0000000516942b21 in WebCore::AsyncScrollingCoordinator::requestScrollPositionUpdate(WebCore::ScrollableArea&, WebCore::IntPoint const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/AsyncScrollingCoordinator.cpp:263
#2  0x00000005170518a2 in WebCore::RenderLayer::scrollToOffset(WebCore::IntPoint const&, WebCore::ScrollType, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayer.cpp:2498

```

Then call `RemoteLayerTreeDrawingArea::flushLayers`, constuct RemoteScrollingCoordinatorTransaction too, encode, then send msg to `Messages::RemoteLayerTreeDrawingAreaProxy::CommitLayerTree`



### UI process:

Get the msg decode, then call `RemoteLayerTreeDrawingAreaProxy::CommitLayerTree`. From RemoteScrollingCoordinatorTransaction, `RemoteScrollingCoordinatorProxy::commitScrollingTreeState` will get the stateTree which will apply to the m_scrollingTree by `m_scrollingTree->commitTreeState`. In `ScrollingTreeScrollingNodeDelegateIOS::commitStateAfterChildren`, it will deal with different actions, like: ScrollContainerLayer, ScrollPosition, ScrollOrigin, etc. For RequestedScrollPosition, it will call  scrollingNode().scrollTo which will change the contentOffset of UI scrollView. 

```
#3  0x0000000121250a6d in -[UIScrollView setContentOffset:] ()
#4  0x0000000114b49ac8 in WebKit::ScrollingTreeScrollingNodeDelegateIOS::repositionScrollingLayers() at /Users/cathiechen/cc/source/WebKit/Source/WebKit/UIProcess/RemoteLayerTree/ios/ScrollingTreeScrollingNodeDelegateIOS.mm:310
#5  0x0000000114a88a14 in WebKit::ScrollingTreeOverflowScrollingNodeIOS::repositionScrollingLayers() at /Users/cathiechen/cc/source/WebKit/Source/WebKit/UIProcess/RemoteLayerTree/ios/ScrollingTreeOverflowScrollingNodeIOS.mm:78
#6  0x00000001294ddc3d in WebCore::ScrollingTreeScrollingNode::currentScrollPositionChanged() at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/ScrollingTreeScrollingNode.cpp:179
#7  0x00000001294ddbc2 in WebCore::ScrollingTreeScrollingNode::scrollTo(WebCore::FloatPoint const&, WebCore::ScrollType, WebCore::ScrollPositionClamp) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/ScrollingTreeScrollingNode.cpp:172
#8  0x0000000114b49789 in WebKit::ScrollingTreeScrollingNodeDelegateIOS::commitStateAfterChildren(WebCore::ScrollingStateScrollingNode const&) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/UIProcess/RemoteLayerTree/ios/ScrollingTreeScrollingNodeDelegateIOS.mm:303
#9  0x0000000114a889e2 in WebKit::ScrollingTreeOverflowScrollingNodeIOS::commitStateAfterChildren(WebCore::ScrollingStateNode const&) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/UIProcess/RemoteLayerTree/ios/ScrollingTreeOverflowScrollingNodeIOS.mm:73
#10 0x00000001294ca8dd in WebCore::ScrollingTree::updateTreeFromStateNode(WebCore::ScrollingStateNode const*, WTF::HashMap<unsigned long long, WTF::RefPtr<WebCore::ScrollingTreeNode, WTF::DumbPtrTraits<WebCore::ScrollingTreeNode> >, WTF::IntHash<unsigned long long>, WTF::HashTraits<unsigned long long>, WTF::HashTraits<WTF::RefPtr<WebCore::ScrollingTreeNode, WTF::DumbPtrTraits<WebCore::ScrollingTreeNode> > > >&, WTF::HashSet<unsigned long long, WTF::IntHash<unsigned long long>, WTF::HashTraits<unsigned long long> >&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/ScrollingTree.cpp:257
#11 0x00000001294ca897 in WebCore::ScrollingTree::updateTreeFromStateNode(WebCore::ScrollingStateNode const*, WTF::HashMap<unsigned long long, WTF::RefPtr<WebCore::ScrollingTreeNode, WTF::DumbPtrTraits<WebCore::ScrollingTreeNode> >, WTF::IntHash<unsigned long long>, WTF::HashTraits<unsigned long long>, WTF::HashTraits<WTF::RefPtr<WebCore::ScrollingTreeNode, WTF::DumbPtrTraits<WebCore::ScrollingTreeNode> > > >&, WTF::HashSet<unsigned long long, WTF::IntHash<unsigned long long>, WTF::HashTraits<unsigned long long> >&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/ScrollingTree.cpp:254
#12 0x00000001294c9ce0 in WebCore::ScrollingTree::commitTreeState(std::__1::unique_ptr<WebCore::ScrollingStateTree, std::__1::default_delete<WebCore::ScrollingStateTree> >) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/scrolling/ScrollingTree.cpp:179
#13 0x0000000114dae270 in WebKit::RemoteScrollingCoordinatorProxy::commitScrollingTreeState(WebKit::RemoteScrollingCoordinatorTransaction const&, WebKit::RemoteScrollingCoordinatorProxy::RequestedScrollInfo&) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/UIProcess/RemoteLayerTree/RemoteScrollingCoordinatorProxy.cpp:92
#14 0x0000000114a7e079 in WebKit::RemoteLayerTreeDrawingAreaProxy::commitLayerTree(WebKit::RemoteLayerTreeTransaction const&, WebKit::RemoteScrollingCoordinatorTransaction const&) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/UIProcess/RemoteLayerTree/RemoteLayerTreeDrawingAreaProxy.mm:214

```

After the contentOffset changed, it will also try to notify the web process, by calling `ScrollingTreeScrollingNodeDelegateIOS::scrollViewDidScroll`, but m_updatingFromStateNode is true, so it won't pass to webcore again.

```
#0  0x0000000114b47d7e in WebKit::ScrollingTreeScrollingNodeDelegateIOS::scrollViewDidScroll(WebCore::FloatPoint const&, bool) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/UIProcess/RemoteLayerTree/ios/ScrollingTreeScrollingNodeDelegateIOS.mm:336
#1  0x0000000114b47cc2 in ::-[WKScrollingNodeScrollViewDelegate scrollViewDidScroll:](UIScrollView *) at /Users/cathiechen/cc/source/WebKit/Source/WebKit/UIProcess/RemoteLayerTree/ios/ScrollingTreeScrollingNodeDelegateIOS.mm:73
#2  0x0000000121269ff3 in -[UIScrollView(UIScrollViewInternal) _notifyDidScroll] ()
#3  0x0000000121250a6d in -[UIScrollView setContentOffset:] ()
#4  0x0000000114b49ac8 in WebKit::ScrollingTreeScrollingNodeDelegateIOS::repositionScrollingLayers() at /Users/cathiechen/cc/source/WebKit/Source/WebKit/UIProcess/RemoteLayerTree/ios/ScrollingTreeScrollingNodeDelegateIOS.mm:310

```

------------

PS:

scrollToAnchor to ScrollAnimator:
```

#0  0x00000005169db807 in WebCore::ScrollAnimator::ScrollAnimator(WebCore::ScrollableArea&)::$_0::operator()(WebCore::FloatPoint&&) const at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/ScrollAnimator.cpp:62
#1  0x00000005169db771 in WTF::Detail::CallableWrapper<WebCore::ScrollAnimator::ScrollAnimator(WebCore::ScrollableArea&)::$_0, void, WebCore::FloatPoint&&>::call(WebCore::FloatPoint&&) at /Users/cathiechen/cc/source/WebKit/WebKitBuild/Debug-iphonesimulator/usr/local/include/wtf/Function.h:52
#2  0x00000005169c0fb7 in WTF::Function<void (WebCore::FloatPoint&&)>::operator()(WebCore::FloatPoint&&) const at /Users/cathiechen/cc/source/WebKit/WebKitBuild/Debug-iphonesimulator/usr/local/include/wtf/Function.h:79
#3  0x00000005169bed1b in WebCore::ScrollAnimationSmooth::animationTimerFired() at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/ScrollAnimationSmooth.cpp:420
#4  0x00000005169c0072 in WebCore::ScrollAnimationSmooth::scroll(WebCore::FloatPoint const&, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/ScrollAnimationSmooth.cpp:83
#5  0x00000005169c574c in WebCore::ScrollAnimator::scrollToOffset(WebCore::FloatPoint const&, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/ScrollAnimator.cpp:104
#6  0x00000005169cfb8b in WebCore::ScrollableArea::scrollToOffsetWithAnimation(WebCore::FloatPoint const&, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/platform/ScrollableArea.cpp:153
#7  0x0000000517051eda in WebCore::RenderLayer::scrollToOffsetWithAnimation(WebCore::IntPoint const&, WebCore::ScrollClamping) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayer.cpp:2528
#8  0x00000005170536a8 in WebCore::RenderLayer::scrollRectToVisible(WebCore::LayoutRect const&, bool, WebCore::ScrollRectToVisibleOptions const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderLayer.cpp:2718
#9  0x0000000517109c53 in WebCore::RenderObject::scrollRectToVisible(WebCore::LayoutRect const&, bool, WebCore::ScrollRectToVisibleOptions const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/rendering/RenderObject.cpp:430
#10 0x000000051679846d in WebCore::FrameView::scrollToAnchor() at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/FrameView.cpp:3162
#11 0x0000000516798003 in WebCore::FrameView::maintainScrollPositionAtAnchor(WebCore::ContainerNode*) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/FrameView.cpp:2213
#12 0x0000000516797c1e in WebCore::FrameView::scrollToAnchor(WTF::String const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/FrameView.cpp:2179
#13 0x000000051679772e in WebCore::FrameView::scrollToFragment(WTF::URL const&) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/page/FrameView.cpp:2122
#14 0x0000000516582e8b in WebCore::FrameLoader::scrollToFragmentWithParentBoundary(WTF::URL const&, bool) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/loader/FrameLoader.cpp:3232
#15 0x0000000516586bc7 in WebCore::FrameLoader::loadInSameDocument(WTF::URL const&, WebCore::SerializedScriptValue*, bool) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/loader/FrameLoader.cpp:1172
#16 0x00000005165945ac in WebCore::FrameLoader::continueFragmentScrollAfterNavigationPolicy(WebCore::ResourceRequest const&, bool) at /Users/cathiechen/cc/source/WebKit/Source/WebCore/loader/FrameLoader.cpp:3194
#17 0x00000005165abcc8 in WebCore::FrameLoader::loadURL(WebCore::FrameLoadRequest&&, WTF::String const&, WebCore::FrameLoadType, WebCore::Event*, WTF::RefPtr<WebCore::FormState, WTF::DumbPtrTraits<WebCore::FormState> >&&, WTF::Optional<WebCore::AdClickAttribution>&&, WTF::CompletionHandler<void ()>&&)::$_4::operator()(WebCore::ResourceRequest const&, WTF::WeakPtr<WebCore::FormState>&&, WebCore::NavigationPolicyDecision) const at /Users/cathiechen/cc/source/WebKit/Source/WebCore/loader/FrameLoader.cpp:1410


```


