# blink/common

1. 这个目录包含common web platform stuff 需要在blink side和browser side共享的文件.

2. blink目录下可以直接依赖该文件，但blink之外只能通过blink/public/common的public header files访问到该文件。

3. 该目录不可依赖blink下的非common stuff，会在BUILD.gn中体现。

4. namespace应该是blink

5. 与blink的其他目录不同，这个目录下应该：

5.1 使用chromium的common types（//base）,而不是blink的（如：wtf）

5.2 follow chromium's common code style guide.
