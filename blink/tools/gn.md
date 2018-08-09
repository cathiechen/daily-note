# GN

GN真是牛逼到不行！！

GN is a meta-build system that generates ninjiaBuild files.

使用的时候直接在命令行敲gn就好了。`depot_tools`有个叫gn的script，它会直接找到二进制文件，并且run。

虽然`depot_tools`加入到了PATH里，但gn还是应该运行在下载下来的chromium的src目录下面。它依赖于里面的文件。

## step by step

创建`tools/gn/tutorial/BUILD.gn`，添加：

```
executable("hello_world") {
  sources = [
    "hello_world.cc",
  ]
}
```

相同目录下，写一个`hello_world.cc`

在src目录，也就是根目录下，创建或修改已有的BUILD.gn

```
group("root") {
  deps = [
    ...
    "//url",
    "//tools/gn/tutorial:hello_world",
  ]
}
```

编译并运行hello world，

```
gn gen out/Default
ninja -C out/Default hello_world
out/Default/hello_world

#也可以
ninja -C out/Default tools/gn/tutorial:hello_world
```

### `static_library`

在`tools/gn/tutorial/BUILD.gn`可以定义一个static library:

```
static_library("hello") {
  sources = [
    "hello.cc",
  ]
}
```

然后让executable依赖于这个library

```
executable("say_hello") {
  sources = [
    "say_hello.cc",
  ]
  deps = [
    ":hello",
  ]
}
```

当然也可以使用绝对路径`//tools/gn/tutorial:hello`

--------

等等！什么是静态库，什么是动态库？
这篇[文章](https://www.cnblogs.com/skynet/p/3372855.html)讲得很好.

1. 静态库

简言之，静态库在编译期间被打包到应用程序中。首先，xx.cc被生成xx.o（`g++ -c xx.cc`），然后用ar把它生成动态库libxx.a（`ar -crv libxx.a xx.o`）

使用静态库：`g++ Test.cc -L../DynamicLibrary -lxx`, 这样就会加到a.out中了。
使用静态库的话，若程序要更新，只能全量更新。还有一个缺点，好像说是浪费内存。。没看懂

2. 动态库

在程序运行时才被载入，可以增量更新。

生成xx.o(`g++ -fPIC -c xx.o`),  xx.o生成libxx.so(`g++ -shared -o libxx.so xx.o`). 还可以合并成一条命令`g++ -fPIC -shared -o libxx.so xx.cpp`

使用：`g++ Test.cc -L../DynamicLibrary -lxx` 可能找不到动态库，需要改`/etc/ld.so.conf`

具体还要多查资料，现在只是理解皮毛。

--------

刚才那个例子，也可以不用静态库，直接把文件包含到source下面

```

executable("say_hello") {
  sources = [
    "say_hello.cc",
    "hello.cc",
  ]

```

这样分层就不明显了。


## settings

```
static_library("hello") {
  sources = [
    "hello.cc",
  ]
  defines = [
    "TWO_PEOPLE",
  ]
}
```

也可以：

```

config("hello_config") {
  defines = [
    "TWO_PEOPLE",
  ]
}

static_library("hello") {
  ...
  configs += [
    ":hello_config",
  ]
}

```

```
static_library("hello") {
  sources = [
    "hello.cc",
  ]
  all_dependent_configs = [
    ":hello_config"
  ]
}

```


## debug

打印

```
static_library("hello") {
  ...
  print(configs)
}

```


`gn desc <build_dir> <targetname>` 如：`gn desc out/Default //tools/gn/tutorial:say_hello`可以查看信息

`gn --tracelog=mylog.trace`可以用trace看耗时。
