# LayoutImage那些事


## 继承关系

LayoutImage : LayoutReplace : LayoutBox...

计算宽高什么都是在layoutReplace中进行。

LayoutImage更多的是处理它与LayoutImageResource之间的关系。

## LayoutImageResource

LayoutImageResource 以及 它的子类LayoutImageResourceStyleImage

LayoutImageResource主要提供访问ImageResourceContent的接口。

LayoutImageResourceStyleImage主要是提供利用StyleImage创建LayoutImageResource的方式，以及处理它与StyleImage之间的关系。如：给StyleImage AddClient, RemoveClient。它的ImageResourceContent通过`ToStyleFetchedImage(style_image_)->CachedImage()`来获取。

## StyleImage

有这些子类：StyleFetchedImage， StyleFetchedImageSet， StyleGeneratedImage， StylePendingImage。
它们都是ImageResourceObserver，也就是说，会收到ImageResourceContent的变动消息。

StyleFetchedImage拥有一个ImageResourceContent，AddClient, RemoveClient都是直接调用ImageResourceContent的AddObserver和RemoveObserver。所以，layoutImage就可以获取到ImageResourceContent的变动信息了。

### StyleGeneratedImage
代表生成的图片，如gradient\cross-fade\paint()?等。对外来说最主要的作用是提供GetImage().
拥有CSSImageGeneratorValue，CSSImageGeneratorValue的子类包含：CSSCrossfadeValue，CSSGradientValue，CSSPaintValue。
以上3个类都各自有的GetImage. CSSGradientValue::GetImage，根据type不同创建出Gradient，如linear, radial, conic等。随后利用gradient创建GradientGeneratedImage。gradient中保存相关数据，在GradientGeneratedImage.draw被调用时，设置相关flag，然后画出来。所以其实不是事先生成图片保存才某个地方，而是记录flag，需要的时候画出来。

## ImageResourceContent

// ImageResourceContent is a container that holds fetch result of an ImageResource in a decoded form.

```

  // Content status and deriving predicates.
  // https://docs.google.com/document/d/1O-fB83mrE0B_V8gzXNqHgmRLCvstTB4MMi3RnVLr8bE/edit#heading=h.6cyqmir0f30h
  // Normal transitions:
  //   kNotStarted -> kPending -> kCached|kLoadError|kDecodeError.
  // Additional transitions in multipart images:
  //   kCached -> kLoadError|kDecodeError.
  // Transitions due to revalidation:
  //   kCached -> kPending.
  // Transitions due to reload:
  //   kCached|kLoadError|kDecodeError -> kPending.
  ResourceStatus content_status_ = ResourceStatus::kNotStarted;

  // 这应该就是decoded form iamge吧
  scoped_refptr<blink::Image> image_;

  // hashset保存着observer
  HashCountedSet<ImageResourceObserver*> observers_;
  HashCountedSet<ImageResourceObserver*> finished_observers_;
```

ImageResourceContent::Fetch`发起图片下载，实现在`ImageResource::Fetch`
这里会提供下载的信息：

```
  void AppendData(const char*, size_t) override;
  void Finish(double finish_time, base::SingleThreadTaskRunner*) override;
  void FinishAsError(const ResourceError&,
                     base::SingleThreadTaskRunner*) override;
```

`ImageResourceContent::UpdateImage`把data传给image。在这里：

```
  if (all_data_received &&
      size_available_ != Image::kSizeAvailableAndLoadingAsynchronously) {
    UpdateToLoadedContentStatus(status);
    NotifyObservers(kShouldNotifyFinish, CanDeferInvalidation::kNo);
  } else {
    NotifyObservers(kDoNotNotifyFinish, CanDeferInvalidation::kNo);
  }
```
这样observer就可以收到消息了，imagechanged， notifyfinished。。。
### ImageResource

负责下载相关的工作


### image
`WebKit/Source/platform/graphics/Image.h`image有两个子类

```
  virtual bool IsSVGImage() const { return false; }
  virtual bool IsBitmapImage() const { return false; }
```
这应该是最终image实现的地方，有draw什么的函数。
