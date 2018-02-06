# Threading and Tasks in Chrome

翻译[这篇](https://chromium.googlesource.com/chromium/src/+/master/docs/threading_and_tasks.md)

## overview

### Threads

每个chrome process有（多进程结构）：

- a main thread
  - browser process里: 更新UI
  - renderer processes里：runs most of Blink
- an IO thread
  - browser process：处理IPC和network请求
  - renderer processes：处理IPCs
- 一些特殊用途的threads
- 一个通用线程池

大多数线程都有一个loop，用于从一个queue中get task，并run。queue可以在多个线程中共享。

### Tasks

A task is a base::OnceClosure，被加到一个queue中，用于异步执行。
base::OnceClosure保存这函数指针和参数。它有个run函数，可以让指定参数（通常是第一个）调用函数指针。通过base::BindOnce创建。见[callback和bind document](https://chromium.googlesource.com/chromium/src/+/master/docs/callback.md).

```
void TaskA() {}
void TaskB(int v) {}

auto task_a = base::BindOnce(&TaskA);
auto task_b = base::BindOnce(&TaskB, 42);
```

多个task可以通过以下方式运行：
- Parallel并行：没有执行顺序，可能在多个线程一次执行完成
- sequenced顺序：在任何一个线程一次执行一个task，顺序：FIFO
- Single Threaded单线程：sequenced + 同一个线程
  - COM Single Threaded: COM Single Threaded + COM initialized

### Prefer Sequences to Threads

在以下场景下，更推荐使用sequence而非Single Threaded：仅需要线程安全，在线程调度时不会出现其它结果。sequenced会跳到其他线程，而不会卡在同一个线程中，被不相关的工作耽误。同时，这也意味着，该机器允许使用多线程。
很错core api对sequence都有兼容。有很少一部分class要求在同一个线程运行，即：使用了该线程的local storage。但以前的代码基本都假设是在单一线程运行的。若你的代码可以用sequenced模式，但又过份依赖于：ThreadChecker/ThreadTaskRunnerHandle/SingleThreadTaskRunner。你应该考虑修正这些依赖。或者之上价格blocking bug到crbug中，然后在代码里加个TODO。
关于如何从single thread改成sequenced，可以看[这里](https://chromium.googlesource.com/chromium/src/+/master/docs/task_scheduler_migration.md)

## Posting a Parallel Task

### Direct Posting to the Task Scheduler

一个可以跑在任何thread，不管任何顺序，或与其他task无关的task应该这样post：`base::PostTask*()`in base/task_scheduler/post_task.h.

```
base::PostTask(FROM_HERE, base::BindOnce(&Task));
```
上面这个post方法使用默认traits。

`base::PostTask*WithTraits()`允许指定TaskTraits。

```
base::PostTaskWithTraits(
    FROM_HERE, {base::TaskPriority::BACKGROUND, MayBlock()},
    base::BindOnce(&Task));
```

### Posting via a TaskRunner
也可以用taskrunner post task

```
class A {
 public:
  A() = default;

  void set_task_runner_for_testing(
      scoped_refptr<base::TaskRunner> task_runner) {
    task_runner_ = std::move(task_runner);
  }

  void DoSomething() {
    // In production, A is always posted in parallel. In test, it is posted to
    // the TaskRunner provided via set_task_runner_for_testing().
    task_runner_->PostTask(FROM_HERE, base::BindOnce(&A));
  }

 private:
  scoped_refptr<base::TaskRunner> task_runner_ =
      base::CreateTaskRunnerWithTraits({base::TaskPriority::USER_VISIBLE});
};
```

后面还很多，不想翻译了。。。
