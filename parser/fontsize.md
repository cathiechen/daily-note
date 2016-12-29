#fontsize
问题源于写td的gtest时，无法通过style正确获取td的fontsize的值，specific和computed都不行；div的却可以正确获取。原因是settings的defaultfontsize没设置，为0，所以FontBuilder::updateSpecifiedSize时算出来的size是-1。嗯，先研究一下fontsize的机制再说。
测试用例：

	<html>
	<body>
		<div id="container"></div>
		<script>
			document.getElementById("container").innerHTML="<style>html{font-size:16px;}</style><table><tr><td>xxx</td></tr></table>";	
		</script>
	</body>
	</html>

##fontDescription
- 是什么：
	- 描述font的状态：各种size, 各种样式
	- 对外接口：提供改变fontDescription状态的接口
- 谁创建：fontbuilder
- 怎么用：字体css变化、scale变化等，都可以取出fd来，设置不同的值，然后再往style设置fd，这样页面就是自动更新
- 保存：styleinheriteddata中保存

##fontbuilder
- 是什么：负责创建和改变fd
	- 各种set
	- 保存各种页面状态, 如: didChangeEffectiveZoom()
	- createFontForDocument: 服务document，创建fd，**设置初始值**。
	- createFont: 服务一般element，获取fd，**设置初始值**。由于这里是初始化的地方，所以如果fontsize出现1.0等奇怪的值，就应该是这里某个逻辑引起的。 createFont什么情况下调用？有些节点不会调用, 如div；有些节点会，如:table\td等。这与节点的默认css样式有关，如果没有设置字体相关的css，走到createfont马上就是reture, 因为flags==0.这就解释了，为什么td获取的fontsize有问题，而div没问题。
- 创建：element的存在StyleResolverState中, document自己设置个局部变量
- 使用：主要是负责创建font

相关代码：

	void FontBuilder::createFont(PassRefPtrWillBeRawPtr<FontSelector> fontSelector, ComputedStyle& style)
	{
    	if (!m_flags)
        	return;

    	FontDescription description = style.fontDescription();
		...
	}

##StyleResolverState
- 是什么：这个应该是保存一个element的style的所有状态
- 创建：局部变量，styleforelement的时候会创建
- 使用：

##有用的堆栈

document的font创建：

	minichrome.exe!blink::FontBuilder::updateSpecifiedSize(blink::FontDescription & fontDescription, const blink::ComputedStyle & style) 行 303	C++
 	minichrome.exe!blink::FontBuilder::createFontForDocument(WTF::PassRefPtr<blink::FontSelector> fontSelector, blink::ComputedStyle & documentStyle) 行 407	C++
 	minichrome.exe!blink::Document::setupFontBuilder(blink::ComputedStyle & documentStyle) 行 1554	C++
 	minichrome.exe!blink::StyleResolver::styleForDocument(blink::Document & document) 行 478	C++
 	minichrome.exe!blink::Document::attach(const blink::Node::AttachContext & context) 行 2116	C++
 	minichrome.exe!blink::LocalDOMWindow::installNewDocument(const WTF::String & mimeType, const blink::DocumentInit & init, bool forceXHTML) 行 349	C++

StyleResolverState创建：

	minichrome.exe!blink::StyleResolverState::StyleResolverState(blink::Document & document, const blink::ElementResolveContext & elementContext, const blink::ComputedStyle * parentStyle) 行 43	C++
 	minichrome.exe!blink::StyleResolver::styleForElement(blink::Element * element, const blink::ComputedStyle * defaultParent, blink::StyleSharingBehavior sharingBehavior, blink::RuleMatchingBehavior matchingBehavior) 行 549	C++
 	minichrome.exe!blink::Document::inheritHtmlAndBodyElementStyles(blink::StyleRecalcChange change) 行 1566	C++
 	minichrome.exe!blink::Document::updateStyle(blink::StyleRecalcChange change) 行 1817	C++
 	minichrome.exe!blink::Document::updateLayoutTree(blink::StyleRecalcChange change) 行 1759	C++
 	minichrome.exe!blink::Document::updateLayoutTreeIfNeeded() 行 431	C++
 	minichrome.exe!blink::FrameView::updateStyleAndLayoutIfNeededRecursive() 行 2529	C++
 	minichrome.exe!blink::FrameView::updateAllLifecyclePhasesInternal() 行 2476	C++
 	minichrome.exe!blink::FrameView::updateAllLifecyclePhases() 行 2452	C++
 	minichrome.exe!blink::PageAnimator::updateLayoutAndStyleForPainting(blink::LocalFrame * rootFrame) 行 98	C++
 	minichrome.exe!blink::PageWidgetDelegate::layout(blink::Page & page, blink::LocalFrame & root) 行 67	C++
 	minichrome.exe!blink::WebViewImpl::layout() 行 1916	C++

element的font创建：

	minichrome.exe!blink::FontBuilder::createFont(WTF::PassRefPtr<blink::FontSelector> fontSelector, blink::ComputedStyle & style) 行 389	C++
 	minichrome.exe!blink::StyleResolver::updateFont(blink::StyleResolverState & state) 行 857	C++
 	minichrome.exe!blink::StyleResolver::applyMatchedProperties(blink::StyleResolverState & state, const blink::MatchResult & matchResult) 行 1330	C++
 	minichrome.exe!blink::StyleResolver::styleForElement(blink::Element * element, const blink::ComputedStyle * defaultParent, blink::StyleSharingBehavior sharingBehavior, blink::RuleMatchingBehavior matchingBehavior) 行 611	C++
 	minichrome.exe!blink::Document::inheritHtmlAndBodyElementStyles(blink::StyleRecalcChange change) 行 1566	C++
