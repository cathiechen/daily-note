# misc

关于架构的东西，看完之后，很容易就忘记了。在这里记录一下。

## 关于thread

在同一个process中，并行执行一些task。task是一个必包，`base::BindOnce`，把函数和作用的object及参数，打包在一起，形成一个可以独立运行的环境。

task可以post到不同的taskrunner中。这些taskrunner，可以制定在哪个线程运行，优先级等等。

在browser process中， mainthread和threadIO一般都比较忙，不是很相关的话，优先使用其他thread。

Thread提供并行运算。

LayoutNG把layout碎片化后，为并行提供可行性，处理流程可以这样：

1. 网页分块
2. 每块启动一个线程Layout
3. 每个线程Layout结束后，发送完成layout消息
4. 收到所有分块完成Layout消息后，就可以开始paint了
5. paint

实验了一下，不同frame放到不同线程处理style和Layout，但是失败。处理过程中，会DCHECK `Platform::Current()->CurrentThread()`. common thread这里好像是空的。

其他的，不是很重的test，可以成功。


## 多进程架构

Browser Process负责UI相关。 Render process负责内容显示相关。

BP和RP通过IPC系统通信。

RenderProcessHost和RenderProcess， RenderViewHost和RenderView，一一对应通信。

存在share process的情况。

sandboxing the renderer process: 是指限制的rp的权限:file system, networt, etc.

### process model:

四种model：

1. process-per-site-instance
2. process-pre-site
3. process-per-tab
4. single process

不同站点隔离，会使浏览器更安全。但存在怎么隔离和消耗的问题。


## 线程内部（blink）的通信

## IPC进程间通信
