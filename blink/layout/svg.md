# svg

很久以前就对svg很好奇，但这些年一直没看过。我也不知道自己在忙什么？！

实现image marker时，刚好遇到一个svg相关的bug。所以，一天就把之前几年都没学的知识点学了。

## svg使用

推荐这个[网站](https://developer.mozilla.org/zh-CN/docs/Web/SVG/Tutorial)作为教程

```

<svg version="1.1"
     baseProfile="full"
     width="300" height="200"
     xmlns="http://www.w3.org/2000/svg">

  <rect width="100%" height="100%" fill="red" />

  <circle cx="150" cy="100" r="80" fill="green" />

  <text x="150" y="125" font-size="60" text-anchor="middle" fill="white">SVG</text>

</svg>

```



## svg节点

### element节点

根节点是`SVGElement : public Element`。位于：core/svg/SVGElement.h


### layout节点

构建layout tree时，首先会创建一个LayoutSVGRoot，作为根节点。

## image url 是svg

e.g.：


```

<style>
  ul { font-family: Ahem;
  }
  ul li {
      font-size: 250px;
      color: green;
      list-style-position: inside;
      line-height: 1em;
  }
  ul li.svg {
      list-style-image: url("data:image/svg+xml,<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 1 1'><rect width='1' height='1' fill='green'/></svg>");
  }
  ul li.png {
      list-style-image: url("./green.png");
  }
</style>
<body style="zoom:2;">
<ul><li class="svg"></ul>
</body>


```


WebKit/Source/core/svg/graphics/SVGImage.cpp 是image的子类。另一个是bitmapImage。
SVGImage创建一个page，然后把svg的数据，传到这个page的frame，对其解析，并创建dom tree，layout tree等等。
SVGImage会保持这个page，这些过程都在SVGImage::DataChanged中处理。


```
#0  0x00007fffea404354 in blink::LayoutSVGRect::LayoutSVGRect(blink::SVGRectElement*) (this=0x36de1fe5c128, node=0x148b384cf038) at ../../third_party/WebKit/Source/core/layout/svg/LayoutSVGRect.cpp:36
#1  0x00007fffea7d0a17 in blink::SVGRectElement::CreateLayoutObject(blink::ComputedStyle const&) (this=0x148b384cf038) at ../../third_party/WebKit/Source/core/svg/SVGRectElement.cpp:156
#2  0x00007fffe98e4a71 in blink::LayoutTreeBuilderForElement::CreateLayoutObject() (this=0x7fffba326ad8) at ../../third_party/WebKit/Source/core/dom/LayoutTreeBuilder.cpp:114
#3  0x00007fffe98b850f in blink::LayoutTreeBuilderForElement::CreateLayoutObjectIfNeeded() (this=0x7fffba326ad8) at ../../third_party/WebKit/Source/core/dom/LayoutTreeBuilder.h:103
#4  0x00007fffe98a7930 in blink::Element::AttachLayoutTree(blink::Node::AttachContext&) (this=0x148b384cf038, context=...) at ../../third_party/WebKit/Source/core/dom/Element.cpp:1929
#5  0x00007fffea771567 in blink::SVGElement::AttachLayoutTree(blink::Node::AttachContext&) (this=0x148b384cf038, context=...) at ../../third_party/WebKit/Source/core/svg/SVGElement.cpp:86
#6  0x00007fffe981d6a9 in blink::ContainerNode::AttachLayoutTree(blink::Node::AttachContext&) (this=0x148b384cee50, context=...) at ../../third_party/WebKit/Source/core/dom/ContainerNode.cpp:946
#7  0x00007fffe98a7b46 in blink::Element::AttachLayoutTree(blink::Node::AttachContext&) (this=0x148b384cee50, context=...) at ../../third_party/WebKit/Source/core/dom/Element.cpp:1962
#8  0x00007fffea771567 in blink::SVGElement::AttachLayoutTree(blink::Node::AttachContext&) (this=0x148b384cee50, context=...) at ../../third_party/WebKit/Source/core/svg/SVGElement.cpp:86
#9  0x00007fffea7d48a7 in blink::SVGSVGElement::AttachLayoutTree(blink::Node::AttachContext&) (this=0x148b384cee50, context=...) at ../../third_party/WebKit/Source/core/svg/SVGSVGElement.cpp:522
#10 0x00007fffe9903506 in blink::Node::ReattachLayoutTree(blink::Node::AttachContext&) (this=0x148b384cee50, context=...) at ../../third_party/WebKit/Source/core/dom/Node.cpp:1094
#11 0x00007fffe98aa5d0 in blink::Element::RebuildLayoutTree(blink::WhitespaceAttacher&) (this=0x148b384cee50, whitespace_attacher=...) at ../../third_party/WebKit/Source/core/dom/Element.cpp:2322
#12 0x00007fffe9849d6b in blink::Document::UpdateStyle() (this=0x148b384ce060) at ../../third_party/WebKit/Source/core/dom/Document.cpp:2227
#13 0x00007fffe9845937 in blink::Document::UpdateStyleAndLayoutTree() (this=0x148b384ce060) at ../../third_party/WebKit/Source/core/dom/Document.cpp:2133
#14 0x00007fffe985cf40 in blink::Document::FinishedParsing() (this=0x148b384ce060) at ../../third_party/WebKit/Source/core/dom/Document.cpp:5783
#15 0x00007fffea8c7f8f in blink::XMLDocumentParser::end() (this=0x3a39b6f1d308) at ../../third_party/WebKit/Source/core/xml/parser/XMLDocumentParser.cpp:405
#16 0x00007fffea8c84f7 in blink::XMLDocumentParser::Finish() (this=0x3a39b6f1d308) at ../../third_party/WebKit/Source/core/xml/parser/XMLDocumentParser.cpp:420
#17 0x00007fffe9c6ec87 in blink::LocalFrame::ForceSynchronousDocumentInstall(WTF::AtomicString const&, scoped_refptr<blink::SharedBuffer>) (this=0x3a39b6f18ce8, mime_type=..., data=...)
    at ../../third_party/WebKit/Source/core/frame/LocalFrame.cpp:1238
#18 0x00007fffea816d9a in blink::SVGImage::DataChanged(bool) (this=0x2079e87940f0, all_data_received=true) at ../../third_party/WebKit/Source/core/svg/graphics/SVGImage.cpp:793
#19 0x00007fffe75f6191 in blink::Image::SetData(scoped_refptr<blink::SharedBuffer>, bool) (this=0x2079e87940f0, data=..., all_data_received=true)
    at ../../third_party/WebKit/Source/platform/graphics/Image.cpp:118
#20 0x00007fffea4ddd9f in blink::ImageResourceContent::UpdateImage(scoped_refptr<blink::SharedBuffer>, blink::ResourceStatus, blink::ImageResourceContent::UpdateImageOption, bool, bool) (this=0xeeaf43ea208, data=..., status=blink::kPending, update_image_option=blink::ImageResourceContent::kUpdateImage, all_data_received=true, is_multipart=false)
    at ../../third_party/WebKit/Source/core/loader/resource/ImageResourceContent.cpp:440
#21 0x00007fffea4d7916 in blink::ImageResource::UpdateImage(scoped_refptr<blink::SharedBuffer>, blink::ImageResourceContent::UpdateImageOption, bool) (this=0x3a39b6f18008, shared_buffer=..., update_image_option=blink::ImageResourceContent::kUpdateImage, all_data_received=true) at ../../third_party/WebKit/Source/core/loader/resource/ImageResource.cpp:700
#22 0x00007fffea4d8857 in blink::ImageResource::Finish(double, base::SingleThreadTaskRunner*) (this=0x3a39b6f18008, load_finish_time=0, task_runner=0x23ee94186440)
    at ../../third_party/WebKit/Source/core/loader/resource/ImageResource.cpp:416
#23 0x00007fffe7984e99 in blink::ResourceFetcher::ResourceForStaticData(blink::FetchParameters const&, blink::ResourceFactory const&, blink::SubstituteData const&) (this=0x3a39b6f14890, params=..., factory=..., substitute_data=...) at ../../third_party/WebKit/Source/platform/loader/fetch/ResourceFetcher.cpp:474
#24 0x00007fffe79891c4 in blink::ResourceFetcher::RequestResourceInternal(blink::FetchParameters&, blink::ResourceFactory const&, blink::SubstituteData const&) (this=0x3a39b6f14890, params=..., factory=..., substitute_data=...) at ../../third_party/WebKit/Source/platform/loader/fetch/ResourceFetcher.cpp:732
#25 0x00007fffe7988aec in blink::ResourceFetcher::RequestResource(blink::FetchParameters&, blink::ResourceFactory const&, blink::ResourceClient*, blink::SubstituteData const&) (this=0x3a39b6f14890, params=..., factory=..., client=0x0, substitute_data=...) at ../../third_party/WebKit/Source/platform/loader/fetch/ResourceFetcher.cpp:680
#26 0x00007fffea4d6aad in blink::ImageResource::Fetch(blink::FetchParameters&, blink::ResourceFetcher*) (params=..., fetcher=0x3a39b6f14890)
    at ../../third_party/WebKit/Source/core/loader/resource/ImageResource.cpp:168
#27 0x00007fffea4dbe0d in blink::ImageResourceContent::Fetch(blink::FetchParameters&, blink::ResourceFetcher*) (params=..., fetcher=0x3a39b6f14890)
    at ../../third_party/WebKit/Source/core/loader/resource/ImageResourceContent.cpp:120
#28 0x00007fffea743be5 in blink::StyleFetchedImage::StyleFetchedImage(blink::Document const&, blink::FetchParameters&) (this=0x3a39b6f17f50, document=..., params=...)
    at ../../third_party/WebKit/Source/core/style/StyleFetchedImage.cpp:40
#29 0x00007fffe95866f3 in blink::StyleFetchedImage::Create(blink::Document const&, blink::FetchParameters&) (document=..., params=...) at ../../third_party/WebKit/Source/core/style/StyleFetchedImage.h:45
#30 0x00007fffe9586143 in blink::CSSImageValue::CacheImage(blink::Document const&, blink::FetchParameters::PlaceholderImageRequestType, blink::CrossOriginAttributeValue) (this=0x1566bb9e4148, document=..., placeholder_image_request_type=blink::FetchParameters::kDisallowPlaceholder, cross_origin=blink::kCrossOriginAttributeNotSet) at ../../third_party/WebKit/Source/core/css/CSSImageValue.cpp:80
#31 0x00007fffe97ab121 in blink::ElementStyleResources::LoadPendingImage(blink::ComputedStyle*, blink::StylePendingImage*, blink::FetchParameters::PlaceholderImageRequestType, blink::CrossOriginAttributeValue) (this=0x7fffba32dbf0, style=0x2079e8717410, pending_image=0x127e0a1049a0, placeholder_image_request_type=blink::FetchParameters::kDisallowPlaceholder, cross_origin=blink::kCrossOriginAttributeNotSet)
    at ../../third_party/WebKit/Source/core/css/resolver/ElementStyleResources.cpp:146


```


后来画的时候，创建一个PaintRecordBuilder，然后update lifecycle，然后`builder.EndRecording(*canvas);`，大概这样就会把svg里的record画到canvas上吧。

```

  PaintRecordBuilder builder(nullptr, nullptr, paint_controller_.get());

  view->UpdateAllLifecyclePhasesExceptPaint();
  view->PaintWithLifecycleUpdate(builder.Context(), kGlobalPaintNormalPhase,
                                 CullRect(bounds));
  DCHECK(!view->NeedsLayout());

  if (canvas) {
    builder.EndRecording(*canvas);
    return nullptr;
  }
  return builder.EndRecording();

```
