# [Shaking Issue](https://codereview.chromium.org/2817443003/)性能问题

## select-long-word

- 测试内容：测试`sel.modify("extend", "forward", "word")`
- 流程：modify的时间主要花在找下一个word上。寻找的算法是这样的：遍历当前node的textinlinebox， 查找这些textinlinebox中是否存在可break的字符。最后，返回position。
- 分析：字体变大，导致textinlinebox变多，最后造成性能下降的假象。

## multicol_lots-of-text-balanced

- 测试内容：测试multicolumn的balance效率
- 现象： 原本不放大的字体被放大了
- 解决方法： multicolumn 需要自己独立成一个cluster，并且在计算宽度的时候应该考虑到分列的情况
- 犹豫： 明知道它里面的内容是一致的，但它一个column的宽度很小，我们需要用哪个宽度？container or column？
