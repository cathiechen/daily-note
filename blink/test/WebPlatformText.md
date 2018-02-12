## web platform test

blink里，wpt在这个目录下，`src/third_party/WebKit/LayoutTests/external/wpt`。
理论上应该用layout test跑的，因为里面应该有ref test。但实际上，layout test很不方便，不利于测试，所以只跑js。
说是，开了dev tool就可以跑。挺好奇怎么跑的。

测试文档都挺老的。可以试试https://github.com/w3c/web-platform-tests 这里的文档。据说，挺坑的。起个server，./wpt run chrome

谷歌现在正在做的：
1. wpt
2. mdi文档
3. 网站：wpt.fyi
4. confluence: https://web-confluence.appspot.com/#!/ 类似canIuse

了解一下chrome driver， dev tool等
