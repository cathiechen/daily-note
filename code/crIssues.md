# CR过程中提到的错误

CR过程中提到很多很初级的错误，记录下来，尽量以后不犯这种错误：

1. 单词编写错误，语法错误（不习惯英文表达的缘故）
2. 函数或变量命名更正（跟英文有关，但感觉还是代码写太少了）
3. 逻辑上的问题
4. 相似函数合并
5. 无用函数移除
6. 注释的写法
7. 只用一次的变量，在用的时候调用
8. 头文件中，基础类型（如：bool, int等），应该带上名字
9. 使用变量代替多次调用
10. include <> 在include ""之后
11. if... else if ... else 的{}，要么全有，要么全没有！
12. 函数尽早return
13. if (xxx == 0)写成if (!xxx)

14. argument ordering wrong. const references / values, then mutable pointers.
