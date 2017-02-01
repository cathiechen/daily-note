#google test
[gtest github](https://github.com/google/googletest)  
[中文学习网站](http://www.cnblogs.com/coderzh/archive/2009/04/12/1434155.html)

##gtest 是如何实现的
- unittest: 单例, 全局只有一个. 负责保存和运行testcases.
- testinfo： 每个TEST或TEST_F由两部分组成：test class和一个全局的变量testinfo。testinfo包含函数名字之类的信息。还包含一个重要的成分：factory，可以创建出一个testcase
- testcase： 这个是一个test class，包含testbody，这个是我们写的测试的具体实现

##总结

gtest设计是很简单很巧妙的。但由于很好学习开源代码的经验，所以还是晕了很久。主要困扰我的问题：

- gtest是如何启动的？其实上就是每写一个TEST，都会生成一个全局变量testinfo，这个全局变量的作用就是把testcase注册到unittest中
- unittest是如何运行testcase的？ 
  testinfo被注册到一个testcases_的container中，运行testcase时，取出testcase通过factory new一个test class。这个class的名字是拼接起来的，独一无二。然后在运行个test class的run，其实就是testbody。