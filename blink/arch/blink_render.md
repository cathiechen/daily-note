# [blink/render](https://cs.chromium.org/chromium/src/third_party/blink/renderer/README.md)

## 文件结构
  
  renderer目录几乎包含了所有 web platform(下文用wp代替)的实现，一定运行在render进程。相比之下，blink/common和blink/public/common也会运行在browser进程中。

  blink/render负责实现blink，不该在blink之外access，需要的话，要通过blink/public

### core目录

  core是实现wp核心和idl interface。由于历史原因，core包含了很多复杂相互依赖的feature，因此core可以被视为单一的整体。

### modules目录

  modules从core中分离出来，包含一系列self-contained, well-defined features. 这些feature follow以下原则：

 - large, tens to hundreds of files, with rare exceptions;
 - self-contained with fine-grained responsibilities and `README.md`;
 - have dependencies outlined with DEPS explicitly;
 - can depend on other features under `platform/`, `core/` or `modules/`, forming a healthy dependency tree.

满足以上特性的才可以放到module目录中。

### platform目录

也是从core中分离出来，包含一系列low level feature。除了依赖性，其它原则几乎与module一样

 - large, tens to hundreds of files, with rare exceptions;
 - self-contained with fine-grained responsibilities and `README.md`;
 - have dependencies outlined with DEPS explicitly;
 - can depend on other features under `platform/` (but not `core/` or `modules/`),
   forming a healthy dependency tree.

platform更底层，不依赖与core和modules.

例子：platform/scheduler, 实现blink内部的task调度。 platform/wtf, 实现container.

### core, modules, platform分离

在specs中没有分开，分开只是为了不让所有的东西都堆在core中。这样可以加强模块化，减少build的时间。

- 与HTML, CSS, DOM强相关的，放在core
- 概念上依赖与core的，放在modules
- core依赖的，放在platform

### bindings目录

包行重度使用v8 API的文件。 v8 API很负责，所以要把他们分开。

### controller目录

包含使用和drive core的文件。为了分层，drive core和impletement for embedder放在controller中。

controller可以依赖于core， modules， platform。 反之，不行！

### devtools 和 build

包含相关的script


## dependencies

依赖顺序图：

Dependencies only flow in the following order:

- `public/web`
- `controller/`
- `modules/` and `bindings/modules`
- `core/` and `bindings/core`
- `platform/`
- `public/platform`
- `public/common`
- `//base`, V8 etc.

See [this diagram](https://docs.google.com/document/d/1yYei-V76q3Mb-5LeJfNUMitmj6cqfA5gZGcWXoPaPYQ/edit).


### type dependencies

非常不推荐在类成员变量中使用下列类型：

- STL string和container， 用wtf的
- GURL和URL::Origin. 用KURL和`SecurityOrigin`
- base里的type，在wtf中都有，wtf优先

base用于blink/render之外的代码。

只要不是成员变量，render中可以使用STL。如：在调用net函数时，使用std::string，存到成员时，转成wtf::string.

base和wtf之间的冗余将会越来越小，这样可以让chromium和blink代码一致。但blink的特性是不可避免的， (e.g., `Vector`, `HashTable`, `AtomicString`).


例外：

- common目录下stl, base优先
- Selected types in `public/platform` and `public/web`,
  whole purpose of which is conversion between WTF and STL,
  for example `WebString` or `WebVector`.

### Mojo

`core/`, `modules/`, `bindings/`, `platform/` and `controller/`可以使用mojo直接和browser进程对话。这样可以减少不必要的public api和abstraction layers。非常推荐。
