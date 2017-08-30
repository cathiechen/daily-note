# style

LayoutObect有好几个方式可以取到style： 

```
  const ComputedStyle* Style() const { return style_.Get(); }
  ComputedStyle* MutableStyle() const { return style_.Get(); }

  // style_ can only be nullptr before the first style is set, thus most
  // callers will never see a nullptr style and should use StyleRef().
  // FIXME: It would be better if style() returned a const reference.
  const ComputedStyle& StyleRef() const { return MutableStyleRef(); }
  ComputedStyle& MutableStyleRef() const {
    DCHECK(style_);
    return *style_;
  }
```
通过`MutableStyleRef()`和`MutableStyle()`可以改变style，但要注意了！ 这种方式应该是不被推荐的！这样改变的style，只是偷偷地改，准确地说layoutobject不知道你改了什么，或者说，有可能出现改变的东西没应用上的情况。比如：在styledidchange的时候改变display，但没有经过setstyle，所以这个应该影响到layoutobject的css，将没有被应用到。详见，`LayoutObject::SetStyle`：

正确的用法：
```
  LayoutBlock* marker_container = CreateAnonymousBlock();
  RefPtr<ComputedStyle> container_style = ComputedStyle::Create();

  // 对style处理
  container_style->InheritFrom(StyleRef());

  container_style->SetDisplay(EDisplay::kBlock);
  container_style->SetPosition(EPosition::kRelative);
  container_style->SetHeight(Length(0, kFixed));

  marker_container->SetStyle(std::move(container_style));
```
