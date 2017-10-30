# LayoutNG

[LayoutNG]("https://chromium.googlesource.com/chromium/src/+/master/third_party/WebKit/Source/core/layout/ng/README.md")的翻译。

## 总体
layout mode由display控制（还有custom layout），不同类型的layout有不同的NGLayoutAlgorithm.
layout需要一下参数：
1. NGBlockNode：这个代表要layout的节点。
	1.1 该节点的style
	1.2 该节点的孩子列表
2. NGConstraintSpace：代表layout产生NGPhysicalFragment的空间
3. BreakTokens：每次layout完一个节点都应该走到BreakTokens中
除此之外，layout不应该再访问其它信息。

## Fragment Caching
layout之后，会cache结果，以免下次需要重做layout。
1. 在LayoutNGBlockFlow中，保存constraint space 和 resulting fragment。为了简化，只有在没有break tonken的情况下才保存。保存结果接口：LayoutNGBlockFlow::SetCachedLayoutResult
2. 若已cached，layout之前可以取到值，layout就好了。通过这个接口：LayoutNGBlockFlow::CachedLayoutResult
3. CachedLayoutResult会clone fragment，但没有offset。这样父亲节点就可以定位这个fragment了
4. 只有满足下列条件才会重用：
	4.1 没有break token
	4.2 不需要重排的节点，还是needslayout。
