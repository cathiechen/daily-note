# LayoutTest

## TestType
- js 
	- 用的是[XUnit tests](https://en.wikipedia.org/wiki/XUnit)测试框架。这是什么框架，见下文！
	- 使用[testharness.js](https://github.com/w3c/testharness.js)框架，这个是通用的。在浏览器厂商中共享。
- reference Text
	- test和ref page，用pixel by pixel的方式对比输出网页的截图。不好，很慢。但测试paint的时候常用。
- pixel Test
	- test page和picture对比，更不好，因为随着环境的变化，图片结果经常发生变化。经常用layout tree对比代替
- layout tree test
	- 对比layout tree


## 如何rebaseline测试结果
- `git cl try`
- `webkit-patch rebaseline-cl`
- ps:成为committer先，咩哈哈...[详见](https://groups.google.com/a/chromium.org/forum/#!starred/blink-dev/A4HwldX-wFo)


## [XUnit tests](https://en.wikipedia.org/wiki/XUnit) 框架
- test runner, 可以测试测试用例并输出结果的程序。当然测试用例按xunit框架写的。
- test case，测试用例
- test fixture， 测试用例需要用到的上下文
- test execution， 开始有setup，结束时teardown

		setup(); /* First, we should prepare our 'world' to make an isolated environment for testing */
		...
		/* Body of test - Here we make all the tests */
		...
		teardown(); /* At the end, whether we succeed or fail, we should clean up our 'world' to 
		not disturb other tests or code */
- test result formatter, 除了plaint，human-readable的format， 还有一种xml的format
- Assertions, 这个经常用到。ture->correct; false->abort this case

## 相关文档
实在看不下去了，需要的时候再继续吧...

- [Layout Tests](https://chromium.googlesource.com/chromium/src/+/master/docs/testing/layout_tests.md)
- [web-platform-tests](https://chromium.googlesource.com/chromium/src/+/master/docs/testing/web_platform_tests.md)
- [Writing Layout Tests](https://chromium.googlesource.com/chromium/src/+/master/docs/testing/writing_layout_tests.md)
- [Layout Test Expectations and Baselines](https://chromium.googlesource.com/chromium/src/+/master/docs/testing/layout_test_expectations.md)
	- NerverFixedTests 不解决
	- TestExpectation 先不解决
	- 如何rebaseline？
		- `git cl try` 
		- TestExpectation [NeedsRebaseline]，这个的原理是：有专门的robot扫描TestExpectation，并commit rebaseline，然后去除[NeedsRebaseline]
		- [needsManualrebaseline]
	- Smoke tests：在安卓上运行
	- 语法：[bugs]["["modifiers"]"] test name ["["expectations"]"]
- [w3c 的 testharness.js](https://github.com/w3c/testharness.js)
	- 一个测试框架
	- 页面内包含testharness.js就可以使用它提供的一些api，比如：各种assert
	- 为了保证扩展性，import testharness.js后，还会添加一个testharnessreport.js，给浏览器重载用的
	- API：各种assert，id=“log”会被写上测试结果。