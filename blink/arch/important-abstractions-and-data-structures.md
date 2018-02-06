# 主要的抽象和数据结构

翻译[这篇文章](https://www.chromium.org/developers/coding-style/important-abstractions-and-data-structures)

## TaskRunner & SequencedTaskRunner & SingleThreadTaskRunner
用于posting`base::Callbacks` task 的接口。不保证顺序执行，甚至不保证执行。SequencedTaskRunner保证顺序执行（基本上是FIFO）。SingleThreadTaskRunner保证顺序执行在同一个线程上。MessageLoopProxy就是一个SingleThreadTaskRunner。这些接口在测试依赖相关的task是很有用的。NOTE:不保证执行，有个非常好用的函数叫PostTaskAndReply()，可以在执行结束后发送一个reply到原来的taskrunner。

## MessageLoop & MessageLoopProxy & BrowserThread & RunLoop
这些API都是用于posting task的。MessageLoop是MessageLoopProxy一个具体的obj（在chrome中广泛使用）。在使用过程中，应该总是使用MessageLoopProxy而不是MessageLoop。或者如果在chrome/或content/目录，则可以使用BrowserThread。这样做是为了避免MessageLoop的销毁竞争。因为若底层的MessageLoop被销毁，MessageLoopProxy和BrowserThread会删除task。NOTE：posting task to MessageLoopProxy，不代表task会被执行。

PS：这里有个关于什么时候使用SequencedTaskRunner vs MessageLoopProxy vs BrowserThread的争辩。像SequencedTaskRunner这样的接口让代码更abstract/reusable/testable。从另一个角度上说，由于而外的一层封装，导致代码没那么明显（怎么运行的，没那么明显）。使用一个具体的BrowserThread ID 让其可以明显地显示在哪个thread运行。当然你可能会争辩，改个更恰当的名字，让其可读性更强。目前的决定是必要的时候将BrowserThread变成TaskRunner的子类。MessageLoopProxy应该总被当成SingleThreadTaskRunner或SequencedTaskRunner的父接口。

## base::SequencedWorkerPool & base::WorkerPool

chromium有2种（现在好像只有base::SequencedWorkerPool了）worker pool。base::SequencedWorkerPool是种复杂的worker pool，继承与TaskRunner。提供按顺序运行task的方法。（通过共享一个SequenceToken）而且可以指定shutdown的行为（在task执行的时候block shutdown。在浏览器还没启动但有shutdown时，不要run 这个task。或者不要在ran task，不管browser shutdown且不block这个shutdown...）SequencedWorkerPool还提供基于SequenceToken返回SequencedTaskRunner的机制。在其他所有thread shutdown后（除了main thread），chromium才关闭base::SequencedWorkerPool。base::WorkerPool是一个global object（现在好像没有了。。）browser shutdown的时候，它也不会被shut down。所以所有运行在上面的task都不会被停掉。用base::WorkerPool是不明智的，因为它可能依赖一些在process shutdown时删除的object。

## base::Callback and base::Bind()

base::Callback是一系列callback的模板（不同的参数和return）。NOTE：这些callback是可以复制的，但the function pointer and the bound arguments一般通过refcounting共享内部存储。base::Bind() 作用是绑定function point和argument，然后返回一个 base::Callback。base::Bind()把function point和argument拷贝到an internal refcounted storage object.

若function是个成员函数，base::bind()会自动对第一个参数（执行这个function的对象）addRef/release.若该参数不是refcounted则会发出警告，用base::WeakPtr or base::Unretained()可以避免该警告。另外对于function argument，bind用`COMPILE_ASSERT`来确保他们是refcount不是raw point。用`scoped_refptrs` or call `make_scoped_refptr()` 可以避免bugs. 另外，base::Bind() 是懂base::WeakPtr的。若function是成员函数且bind的第一个参数是weakptr，bind会加一层封装，确保只在这个weakptr不是NULL时，才调用function。对于其他参数，bind也做了类似处理。
1. `base::Unretained() `，disable上面介绍的refcount和`COMPILE_ASSERT`的限制。小心使用，因为这意味着你应该确保该obj的生命周期可以持续到callback被调用的时候。对于成员函数，最好还是要用weakptr。
2. `base::Owned()`， 把ptr的ownership转移到callback中。非常实用！因为不能确保callback被运行（在shutdown时，会delete这个obj），让callback拥有ptr的ownership，可以避免没有run callback导致的shutdown时的内存泄漏。
3. `base::Passed()`，用于将一个scoped object转移给callback。如：`scoped_ptr`/`ScopedVector`/etc。Owned和Passed的区别：Passed要求函数签名将scoped type作为参数，从而允许通过realese转移ownership。NOTE：因为scopted type的scope是function scope，所以这个callback只能调用一次。否则，可能出现delete两次的错误。由于passed比较复杂，所以你可能更愿意用owned.
4. `base::ConstRef()`，将参数以const reference的方式传递，而非把它拷贝过去。通常不用，因为对参数的生命周期有要求。
5. `base::IgnoreResult()`，将函数指针传给bind，并忽略结果。对于没有参数和return void的closure有用。

## `scoped_refptr<T>` & `base::RefCounted` & `base::RefCountedThreadSafe`

Reference counting偶尔会用到，但这也经常意味着没想清楚ptr的ownership。Reference counting应该只用于ptr的ownership是shared的时候（如：多个tabs共用一个render process）。而不是用于避免理清ptr的life cycle。

## Singleton & base::LazyInstance

全局的，应该避免使用。如果一定要用，最好用base::LazyInstance，而非Singleton。使用这两个类的原因一般是懒得关心创建和delete相反的顺序。他们都在`AtExitManager`创建和delete（顺序相反）。在chromium的browser process中，`AtExitManager`在main thread（UI thread）的前期创建。所以，不管这些obj在哪个线程创建的，所有都在main thread中destroy。更喜欢base::LazyInstance而非Singleton的理由在于：LazyInstance在data segment有预留空间，这样可以避免内存碎片，且重构了new，避免在内存中放置对象。
NOTE：他俩都提供了shutdown时防止内存泄漏的机制，避免关机变慢的问题。

## base::Thread & base::PlatformThread

通常用不到。因为只要pass task到一个已存在的thread就够了。每个thread有一个跑在base::PlatformThread上的message loop。

## base::WeakPtr & base::WeakPtrFactory

Weak Ptr不是线程安全的。若被destoy则返回NULL。传到其它线程去（删除）是安全的，但只能在创建的线程中使用。当base::WeakPtr所指的obj被删除时， base::WeakPtrFactory可以用于自动取消base::Callbacks。

## FilePath

一种跨平台的file path的代表。通常用这个就好了，而非特定平台的file path。

## ObserverList & ObserverListThreadSafe

ObserverList是一个线程不安全的的obj，ObserverList意在于被当成一个类成员变量。它提供了一个简单的封装：封装一堆Observer objects，并提供调用notification接口。
ObserverListThreadSafe类是，包含多个same PlatformThreadId 的ObserverList和Observer Notification，从而允许代理跨线程调用。。。好难翻译啊。。

## Pickle

Pickle提供序列化和反序列化的功能。

## Value

Value用于指定包行simplie value（bool/int/string）的递归的data class（list 和 dictionaries）。这些value可以被序列号为JSON，或反序列化。

## LOG

## FileUtilProxy

一般不该在敏感线程下做IO操作，如：UI和IO线程。你可以通过这些utilities在其他线程（such as BrowserThread::FILE）代理这些操作。

## Time, TimeDelta, TimeTicks, Timer

通常用TimeTicks，而非Time

## PrefService, ExtensionPrefs

保存用户信息和相关的state。

