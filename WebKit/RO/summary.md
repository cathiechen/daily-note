# ResizeObserver


## data stuct

The design is borrowed from WebKit intersectionObserver and blink ResizeObserver.

### document

At first document would delete RO if RO is disconnected. There is a crash issue, if RO is deleted in js callback. The reason is that document is travelling the RO list while invoke the callbacks.

So document will delete RO, when RO is destructed.

## FrameTree changed in js

### Gather documents first

For deliverRO could invoke js, then there is chance that frametree could be changed, and frame, frameview and document might be deleted after js callbacks. So it's not safe to travel frame tree for invoke deliverRO. To avoid this, we could gather the document first. Generally, the document won't be deleted if there is js object. We could keep a weakptr to document, so it's safe after deliverRO.



## multi frames

I didn't know we could get the element from other frame in js. Yes, it could be achived if the frames are not cross-domain. So I have to run my apache for this case. Actually, it's get easier after run the apache server.

At first, I concerned that Observations might store an element from other document, that would be nightmare. However, it turns out to be well. Because both are stored in weakprt, so all we need is to remove and check if it's nullptr.

## How to fulsh in WebKit

I'm not a expert in flush.

For WK2, after layout, repaintReginAccumulator, flush this regin, invalidateContentAndRootView, scheduleDislplay in widget::invalidate, then display, then page::willDisplayPage.

WK1 doesn't go this way. And Page::willDisplayPage won't be invoked. There is WebHTMLView.mm::viewWillDraw, layout and `_flushCompositingChanges`. 

## WebPreferences
### build flags
### test cases

## WK1 and WK2

## Platforms

### xcode
### objective-c

