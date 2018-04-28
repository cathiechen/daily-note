# LayoutNG的paint

好像比老的要好看很多：）

目录：`src/third_party/blink/renderer/core/paint/ng/`

区别：
- object painter和利用fragment paint的区别，NGPaintFragment是对ngphisycalfragment的封装，可以实现隔离。目前只有inline用fragment paint
- paint offset的区别：
ng的inline结构：
- block NGPaintFragment
  - linebox fragment: PaintLineBoxChildren 
    - inline fragment: PaintInlineChildren
      - text fragment: paint text child
ng的paint offset是相对与父亲fragment的。而老的是相对与line box的。

代码实现：
先是block调用objectpainer，然后PaintLineBoxChildren， 然后PaintInlineChildren，然后paint text child或paint automic inline child.
什么是atomicinline：
- display:inline
- IsDisplayReplacedType

```
  static bool IsDisplayReplacedType(EDisplay display) {
    return display == EDisplay::kInlineBlock ||
           display == EDisplay::kWebkitInlineBox ||
           display == EDisplay::kInlineFlex ||
           display == EDisplay::kInlineTable ||
           display == EDisplay::kInlineGrid ||
           display == EDisplay::kInlineLayoutCustom;
  }
```

