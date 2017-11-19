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
- update: 哇，终于取得了try bot access.. 感谢男神：），下面说下如何用吧：
  - 第一遍'third_party/WebKit/Tools/Scripts/webkit-patch rebaseline-cl'，触发几个服务器的try bot
  - 等到所有服务器都跑完后，没有错误，再跑一遍'third_party/WebKit/Tools/Scripts/webkit-patch rebaseline-cl'，就可以把样例下载下来了。


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

## [testharness.js API](http://web-platform-tests.org/writing-tests/testharness-api.html)

说真的C++写习惯了，真的很看不惯js可以在调用的时候，又声明function的！以下是翻译。

testharness.js是个测试框架，提供一些方便的assert API。可用于同步或异步测试feature。

### 基本使用方法

该script可以用于html或svg，以及web workers脚本。（先关注html的使用方法，web workers该文档也有，一下子用不到，就先不理会了。）

首先，在html文件中，应该引用这个两个文件：

```
<script src="/resources/testharness.js"></script>
<script src="/resources/testharnessreport.js"></script>
```

没记错的话，在blink中，这俩文件放在LayoutTest/resources下面。

一个文件可以定义一个或多个test(这里指的应该是test()吧)。某种意义上说，每个test都是原子的，都有自己的结果（pass/fail/timeout/notrun）。每个test有一个或多个assert。若前面的assert fail了，就不会再跑后面的assert。在html中，会生成一个包含测试结果的table，若html中存在`<div id="log">`，结果将会加到这里面；若没有，就会直接加到body。

注意：默认情况下，test必须在load event触发前运行。之后的情况怎么办？see “Determining when all tests are complete”

### 同步test：

用test()创建同步测试：

```
test(test_function, name, properties)
```

`test_function`里包含需要测试的代码。test就是跑function中的代码。

`properties`是一个js obj，用于往test中传递设置参数。目前只支持metadata的格式，原文中也有这个格式的解释，需要的时候可以看看。例子：

```
test(function() {
  assert_true(document.implementation.hasFeature());
}, "hasFeature() with no arguments")
```



### 异步test：

这个会更复杂，因为测试结果会依赖于其它因素。创建test：

```
async_test(name, properties)
```

通过调用step()，可以将assert加到代码中去。结束，调用done。

栗子：

```
// 创建异步test
var t = async_test("DOMContentLoaded");
// 通过step进行assert判断
document.addEventListener("DOMContentLoaded", function() {
  t.step(function() {
    assert_true(e.bubbles, "bubbles should be true");
  });
});
// 测试结束
t.done();
```

另一种写法：

```
async_test(function(t) {
  document.addEventListener("DOMContentLoaded", function() {
    t.step(function() {
      assert_true(e.bubbles, "bubbles should be true");
    });
    t.done();
  });
}, "DOMContentLoaded");
```

`step_func`把addEventListener需要的function和step合并起来：

```
document.addEventListener("DOMContentLoaded", t.step_func(function() {
  assert_true(e.bubbles, "bubbles should be true");
  t.done();
}));
```

`step_func_done`把`step_func`和done合并起来：

```
document.documentElement.addEventListener("DOMContentLoaded",
  t.unreached_func("DOMContentLoaded should not be fired on the document element"));
```

### Promise Tests

Promises是什么？哼，不翻！

### Single Page Tests

整个文件就只有一个test。这样可以省去test的封装。满足一下几点：

- 全篇没有test、asyc_test
- 最后有个done

这样不封装在test里面，也可以用他们的assert了。

```
<!doctype html>
<title>Basic document.body test</title>
<script src="/resources/testharness.js"></script>
<script src="/resources/testharnessreport.js"></script>
<body>
  <script>
    assert_equals(document.body, document.getElementsByTagName("body")[0])
    done()
  </script>
```

喜欢这种用法，简单粗暴！不过无法生成结果table。

### Making assertions

基本格式：

```
assert_something(actual, expected, description)
```

asserts必须要出现在test\asyc_test\Single Page Tests中，否则无法使用！

### Cleanup

在test里面可以通过添加`add_cleanup`来清除当前的状态

```
  test(function() {
    var element = document.createElement("div");
    element.setAttribute("id", "null");
    document.body.appendChild(element);
    this.add_cleanup(function() { document.body.removeChild(element) });
    assert_equals(document.getElementById(null), element);
  }, "Calling document.getElementById with a null argument.");
```

### 剩下的

不想翻了。。。还有timeout，callback等，还有assert list，真的很全。以后用到的时候再补充吧。