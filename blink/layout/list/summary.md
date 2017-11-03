# Summary

这次实现list marker position的过程中，学到了很多东西，记录一下：
- line-layout：以前对inlinebox、rootinlinebox、baseline、firstlinebaseline，fontmetrics.ascent、lineheight等等概念都很模糊。现在清晰了很多:)还有block direction的排版流程
- skipleadingwhitespace：以前也不知道这是什么鬼，还有alwaysrequirelinebox，现在懂了
- collapse margin： 一样
- overflow： 建立概念，添加到overflowrect的区域才会被显示
- 思考问题：不能很快从错误方案中脱离出来，在abs pos上浪费了很多时间。跟进问题时，从测试用例到看代码时间有点过长，尽量缩短一下下：）
- 沟通：cr的时候很蛋疼啊，一开始找不到人cr，几乎是绝望的。其实，自己沟通很有问题，明明eae一直都有知道这个bug，你还找其他人？！最后还是要找他，感谢eae大牛快速反应，不然我真的可能revert之前修改了。内心还是不够坚定，等待结果的时候太着急！
- 验证：第二个bug居然没验证，还有换行，怎样？打脸了吧？！
- 好的地方：发现abs不行后，转回之前的思路，最终找到解决方案。耐心学习了以上所有的知识点。还输出了文档，个人感觉写得还挺好的，koji显然有看明白：）邮件沟通能力有变强一点。
