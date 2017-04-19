# 自动化测试

自动化测试主要分为两种：性能测试、工程测试。

## 性能测试[catapult](https://github.com/catapult-project/catapult)

1. tracing: 这个经常用到，调试性能问题的神器！
2. telemetry：还不是很了解工作原理，好像是用[web-page-replay](https://github.com/chromium/web-page-replay)来测试的。


## 功能测试

功能测试有很多，比如：browserTest, content test, webkit test等等。

此类的测试用例是基于gtest上的，所以，你可以去看gtest那篇文章。


## 测试很多，各取所需

也困扰过，这么多测试，怎么都搞定。其实没必要都要搞定哈。取你需要的即可：

- 经常改动的功能需要测试
- 关注的重要指标需要测试
