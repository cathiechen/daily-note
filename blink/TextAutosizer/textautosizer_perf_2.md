# [Shaking Issue](https://codereview.chromium.org/2817443003/)性能问题

## select-long-word

- 测试内容：测试`sel.modify("extend", "forward", "word")`
- 流程：modify的时间主要花在找下一个word上。寻找的算法是这样的：遍历当前node的textinlinebox， 查找这些textinlinebox中是否存在可break的字符。最后，返回position。
- 分析：字体变大，导致textinlinebox变多，最后造成性能下降的假象。

- 

## multicol_lots-of-text-balanced

- 测试内容：测试multicolumn的balance效率
- 现象： 原本不放大的字体被放大了
- 解决方法： multicolumn 需要自己独立成一个cluster，并且在计算宽度的时候应该考虑到分列的情况
- 犹豫： 明知道它里面的内容是一致的，但它一个column的宽度很小，我们需要用哪个宽度？container or column？

## html5-full-layout

- 一部分由muli-columns引起
- 一部分由于字体放大引起： 由于页面内容很多，无法具体调查哪里性能有问题。通过对autosize打log，加和未加上shaking的修改对比：加（其中一个cluster以宽度600px计算multiplier）；未加(那个cluster的宽度是448px) 加上之前的经验，推断可能是字体放大导致一些功能更加耗时。为了验证这个想法，分别做下面4组实验：去除shaking修改（cluster 448）； 去除shaking修改，强制改cluster宽度600; 加上shaking修改(cluster 600);加上shaking修改，强制改cluster宽度448。
- 通过数据对比，可以得出性能变化只能字体大小有关。（autosize没有新增耗时）



In order to prevent the shaking issue, this patch make root cluster use LayoutView as WidthProvider to cluster multiplier. It might enlarge the font size  a bit.

select-long-word:
In this test case root cluster's WidthProvider is 800px(<div id="long">). After this patch it changed to 980px(LayoutView). So the font size of content changed from 40px to 49px.

'select-long-word' tests the performance of `sel.modify("extend", "forward", "word")`. Finding the end position of word cost the most part of time. The algorithm of 







Reason of regression:
Test case: multicol_lots-of-text-balanced tests the performance of multi-columns balance. During balancing columns height, need to traverse each line of content. If font-size enlarged and more line box generated, the performance would go down.

Why font-size enlarged?
Here is the test case:
<html>
  <body>
    <div style="-webkit-columns:3; orphans:2; widows:2;">
      lots of text......
    </div>
  </body>
</html>

There is only one cluster created by LayoutView in this page. <div> will use the multiplier of LayoutView cluster. After last CL(2817443003: font-size shaking), LayoutView cluster will use itself as width provider, so <div> will be autosized.

Solution:
The layout width difference between multi-columns and its anonymous child is large (anonymous-child-width * column-count = multi-columns-width). multi-columns elements should create its own cluster and calculate own multiplier.
