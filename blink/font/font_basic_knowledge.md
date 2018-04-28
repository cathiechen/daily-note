# font的基本知识点


## FontMetrics
FontMetrics，是指对于指定字号的某种字体，在度量方面的各种属性，其描述参数包括：
- baseline：字符基线
- ascent：字符最高点到baseline的推荐距离
- top：字符最高点到baseline的最大距离
- descent：字符最低点到baseline的推荐距离
- bottom：字符最低点到baseline的最大距离
- leading：行间距，即前一行的descent与下一行的ascent之间的距离


● 是什么？

## 知识点

![note](https://photos.google.com/search/_tra_/photo/AF1QipOaAWW5t-djJXN5BgQeGfyxOs37HTYPsI8y3W8i)


## 有用的站点

- unicode, utf-8等查询：[http://graphemica.com](http://graphemica.com)


## list marker的实现方式

一开始就用画图的方式： https://chromium.googlesource.com/chromium/src/+/d869b93fe74f4d6cb2dd6f6c3e9bf9daee39ba19/third_party/WebKit/WebCore/khtml/rendering/render_list.cpp


## 其他浏览器的实现方式

- firefore也是用画图的方式，见：BulletRenderer::Paint  [dxr.moozillar.org](https://dxr.mozilla.org)

- safari也是用画图的方式，见: RenderListMarker::paint  [trac.webkit.org](https://trac.webkit.org)
