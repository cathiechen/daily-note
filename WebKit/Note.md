## How WK2 trigger display after layout?

```
#0  WebKit::DrawingAreaCoordinatedGraphics::scheduleDisplay (this=0x7f134deb1280) at ../../Source/WebKit/WebProcess/WebPage/CoordinatedGraphics/DrawingAreaCoordinatedGraphics.cpp:641
#1  0x00007f136feaec71 in WebKit::DrawingAreaCoordinatedGraphics::setNeedsDisplayInRect (this=0x7f134deb1280, rect=...)
    at ../../Source/WebKit/WebProcess/WebPage/CoordinatedGraphics/DrawingAreaCoordinatedGraphics.cpp:98
#2  0x00007f136fde4ca9 in WebKit::WebChromeClient::invalidateContentsAndRootView (this=0x558267c014a0, rect=...) at ../../Source/WebKit/WebProcess/WebCoreSupport/WebChromeClient.cpp:537
#3  0x00007f1371b1ff54 in WebCore::Chrome::invalidateContentsAndRootView (this=0x7f134dedf390, updateRect=...) at ../../Source/WebCore/page/Chrome.cpp:90
#4  0x00007f1371d2c39f in WebCore::ScrollView::repaintContentRectangle (this=0x7f12e3600840, rect=...) at ../../Source/WebCore/platform/ScrollView.cpp:1103
#5  0x00007f1371b931f8 in WebCore::FrameView::repaintContentRectangle (this=0x7f12e3600840, r=...) at ../../Source/WebCore/page/FrameView.cpp:2647
#6  0x00007f13722f3cdc in WebCore::RenderView::flushAccumulatedRepaintRegion (this=0x7f12e30004d0) at ../../Source/WebCore/rendering/RenderView.cpp:559
#7  0x00007f13722f56f2 in WebCore::RenderView::RepaintRegionAccumulator::~RepaintRegionAccumulator (this=0x7ffed0df6b10, __in_chrg=<optimized out>) at ../../Source/WebCore/rendering/RenderView.cpp:902
#8  0x00007f1371bc878f in WebCore::FrameViewLayoutContext::layout (this=0x7f12e3600958) at ../../Source/WebCore/page/FrameViewLayoutContext.cpp:208
#9  0x00007f1371b99435 in WebCore::FrameView::updateLayoutAndStyleIfNeededRecursive (this=0x7f12e3600840) at ../../Source/WebCore/page/FrameView.cpp:4345
#10 0x00007f136fe58b12 in WebKit::WebPage::layoutIfNeeded (this=0x7f134deca000) at ../../Source/WebKit/WebProcess/WebPage/WebPage.cpp:1607

```
