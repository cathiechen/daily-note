# [Service Manager](https://chromium.googlesource.com/chromium/src/+/master/services/service_manager/README.md)

通过service manager可以触达一系列的public mojo interface。

有以下几个功能：
1. 代理service instances之间的interface request, services之间的static capability policies。
2. launch and manage service和process的lifecycle
3. 通过user identity隔离 services instances和interface requests
4. 通过跟踪running service instances和暴露一些privileged API 来 query system state

SM为services提供了一系列的mojo interfaces， 在现实应用中，大部分与SM的互动都是通过更简单的C++ client library进行的.

## Mojo Recap

mojo系统具有两个重要的好处：
1. 轻量的message pipe概念， 允许两个endpoints进行通信。
2. binding layer，允许interfaces bind到这些endpoints中。

Mojo message pipes被设计成轻量的，可以在进程间读写通信。大部分情况下，我们开发者不会直接和这些pipes打交道，而是通过bindings types，这些bindings types封装了a bound interface。
为了使用这些bindings，开发者定义了Mojom IDL format的interface。加上一些build magic，这些自动生成的definition可以被c++，java，js referenced。

## services

service是一系列public mojo interface的priviate实现。所以，访问接口的层次是：ms ==> public mojo interface ==> services. 

service的组成：
1. 一系列的public mojo interface的定义
2. service manifest 声明一些能力，这些能力会暴露给其他的mojo interface
3. private implementation code, 负责处理lifecycle events和interface requests，这些都是通过sm驱动的。

sm负责开启新的service instances， 一个service可能有多个运行的instances。通过unique identity区分这些instances。

service's identity由三部分组成：
1. service name: string
2. user ID: GUID string, 每个运行的service instance都有一个特定的ID
3. instance qualifier: string，用于区分同一个user下的不同services

每个service instance只要运行，就必须实现`service_manager.mojom.Service` interface。通常由C++ client library的`service_manager::Service interface`实现。这个interface由sm的msg驱动。用于接受sm从其他services代理过来的interface request。

每个service instance也有一个可以返回到sm中的outgoing link，用于向其他service发起interface request。即：`service_manager.mojom.Connector interface`，c++： `service_manager::Connector`

## A Simple Service Example

创建一个简单的service

### Private Implementation

`service_manager::Service` interface。

`//services/my_service/my_service.h`

```
#include "base/macros.h"
#include "services/service_manager/public/cpp/service.h"

namespace my_service {

class MyService : public service_manager::Service {
 public:
  MyService();
  ~MyService() override;

  // service_manager::Service:
  void OnStart() override;
  void OnBindInterface(const service_manager::BindSourceInfo& source_info,
                       const std::string& interface_name,
                       mojo::ScopedMessagePipeHandle handle) override;
 private:
  DISALLOW_COPY_AND_ASSIGN(MyService);
};

}  // namespace my_service

```


`//services/my_service/my_service.cc`


```
#include "services/my_service/my_service.h"

namespace my_service {

MyService::MyService() = default;

MyService::~MyService() = default;

void MyService::OnStart() {
}

void MyService::OnBindInterface(const service_manager::ServiceInfo& remote_info,
                                const std::string& interface_name,
                                mojo::ScopedMessagePipeHandle handle) {
}

}  // namespace my_service
```


### Main Entry Point

main函数。虽然service通常是embedded在其他process中运行，不需要main函数。但这里加一个，便于启动。

`//services/my_service/my_service_main.cc`


```
#include "services/my_service/my_service.h"
#include "services/service_manager/public/c/main.h"
#include "services/service_manager/public/cpp/service_runner.h"

MojoResult ServiceMain(MojoHandle service_request_handle) {
  return service_manager::ServiceRunner(new MyService).Run(
      service_request_handle);
}
```


### Manifest

Manifest提供给sm，声明需要的和提供的capability

`//services/my_service/manifest.json`


```
{
  "name": "my_service",
  "display_name": "My Service",
  "interface_provider_specs": {
    "service_manager:connector": {}
  }
}
```


### Build Targets

`//services/my_service/BUILD.gn`


```
import("//services/service_manager/public/cpp/service.gni")
import("//services/service_manager/public/service_manifest.gni")

source_set("lib") {
  public = [ "my_service.h" ]
  sources = [ "my_service.cc" ]

  public_deps = [
    "//base",
    "//services/service_manager/public/cpp",
  ]
}

service("my_service") {
  sources = [
    "my_service_main.cc",
  ]
  deps = [
    ":lib",
    "//services/service_manager/public/c",
  ]
}

service_manifest("manifest") {
  name = "my_service"
  source = "manifest.json"
}
```


`my_service`在output目录生成了`my_service.service`。此时，啥都不做。



### OnStart

在service的实现中，保证了首先收到sm的onstart调用。在调用这个method的过程中，可以通过`context()`访问到`service_manager::ServiceContext`，这个obj暴露了一些值：
1. `service_info()`： `service_manager::ServiceInfo`从sm的角度describing the running service. 包含：service的唯一标识`service_manager::Identity`和`service_manager::InterfaceProviderSpec`描述该service可以提供的capability
2. `identity()`： ServiceInfo中的shortcut
3. `connector()`： `service_manager::Connector`， 用于向其他service发起interface request。
例如：可以用logger写个日志啥的。

```
  logger::mojom::LoggerPtr logger;
  context()->connector()->BindInterface("logger", &logger);
  logger->Log("Started MyService!");
```

### OnBindInterface

每次其他service通过connector发起interface request的时候，这个函数都会被调用。只有通过service's manifest的check后，sm才会调用这个function。参数：

1. `remote_info`：发起请求的remote service的`service_manager::ServiceInfo`
2. `interface_name`： require的interface的名字，是string
3. `handle`：interface pipe的`mojo::ScopedMessagePipeHandle`， remote service提供的bind request实现

sm提供了`service_manager::BinderRegistry`类，便于让service来bind incoming interface request。Typesafe binding callbacks 被提前加到`BinderRegistry`中，并且`OnBindInterface`的incoming arguments可以被forward到registry中，这样就可以bind message pipe了。如：


```
namespace {

void BindDatabase(my_service::mojom::DatabaseRequest request) {
  mojo::MakeStrongBinding(std::make_unique<my_service::DatabaseImpl>(),
                          std::move(request));
}

}  // namespace

MyService::MyService() {
  // Imagine |registry_| is added as a member of MyService, with type
  // service_manager::BinderRegistry.

  // The |my_service::mojom::Database| interface type is inferred by the
  // compiler in the AddInterface call, and this effectively adds the bound
  // function to an internal map keyed on the interface name, i.e.
  // "my_service::mojom::Database" in this case.
  registry_.AddInterface(base::Bind(&BindDatabase));
}

void MyService::OnBindInterface(const service_manager::ServiceInfo& remote_info,
                                const std::string& interface_name,
                                mojo::ScopedMessagePipeHandle handle) {
  registry_.BindInterface(interface_name, std::move(handle));
}

```


## Service Manifests

manifest's interface provider spec是一个capability的dictionary。capability spec定义了provides和requires。
1. provides：key：string；value：mojom interface
2. requires: key: remote service; value: capabilities list

最后，每个interface provider spec包含了一个标准的capability，名叫  ：`service_manager:connector`

`//services/my_service/manifest.json`

```
{
  "name": "my_service",
  "display_name": "My Service",
  "interface_provider_specs": {
    "service_manager:connector": {
      "provides": {
        "database": [
          "my_service::mojom::Database"
        ]
      }
    }
  }
}
```


others.json

```
{
  "name": "other_service",
  "display_name": "Other Service",
  "interface_provider_specs": {
    "service_manager:connector": {
      "requires": {
        "my_service": [ "database" ]
      }
    }
  }
}
```





otherServices.cc

```
void OtherService::OnStart() {
  my_service::mojom::DatabasePtr database;
  context()->connector()->BindInterface("my_service", &database);
  database->AddTable(...);
}

```


## Testing


sm提供`service_manager::test::ServiceTest`， 让测试变得简单。这个test跑在sm的background thread中，可以在运行是注入其他service。

`//services/my_service/my_service_unittest.cc`


```
#include "base/bind.h"
#include "base/run_loop.h"
#include "services/service_manager/public/cpp/service_test.h"
#include "path/to/some_interface.mojom.h"

class MyServiceTest : public service_manager::test::ServiceTest {
 public:
  // Our tests run as service instances themselves. In this case each instance
  // identifies as the service named "my_service_unittests".
  MyServiceTest() : service_manager::test::ServiceTest("my_service_unittests") {
  }

  ~MyServiceTest() override {}
}

TEST_F(MyServiceTest, Basic) {
  my_service::mojom::DatabasePtr database;
  connector()->BindInterface("my_service", &database);

  base::RunLoop loop;

  // This assumes DropTable expects a response with no arguments. When the
  // response is received, the RunLoop is quit.
  database->DropTable("foo", loop.QuitClosure());

  loop.Run();
}
```

`//services/my_service/BUILD.gn`


```
import("//services/catalog/public/tools/catalog.gni")
import("//services/service_manager/public/tools/test/service_test.gni")

service_test("my_service_unittests") {
  sources = [
    "my_service_unittest.cc",
  ]
  deps = [
    "//services/my_service/public/interfaces",
  ]
  catalog = ":my_service_unittests_catalog"
}

service_manifest("my_service_unittests_manifest") {
  name = "my_service_unittests"
  manifest = "my_service_unittests_manifest.json"
}

catalog("my_service_unittests_catalog") {
  testonly = true
  embedded_services = [ ":my_service_unittests_manifest" ]
  standalone_services = [ ":manifest" ]
}
```

还要写个manifest

`//services/my_service/my_service_unittests_manifest.json`


```
{
  "name": "my_service_unittests",
  "display_name": "my_service tests",
  "interface_provider_specs": {
    "service_manager:connector": {
      "requires": {
        "my_service": [ "database" ]
      }
    }
  }
}
```

突然又出现了个catalog的东西，在`service_test`中。任何hosts a Service Manager的runtime environment必须提供sm的a catalog of service manifests实现。catalog定义了其recognized全部的services，而且可以用各种方法控制这次services的启动。

现实中避免引入新的unittest binaries。我们有个`service_unittests`的骨架，定义在`//services/BUILD.gn`。这个suite中，有好几例子。

## Service Manager Catalogs

catalog是一系列service manifests的集合，包行 Service Manager的runtime config。



## 笔记


如：[FileService](https://cs.chromium.org/chromium/src/services/file/file_service.h?g=0)

service中提供了多个mojom interface，如：FileService中，提供了`FileSystem`和`LevelDBService`，见[manifest.json](https://cs.chromium.org/chromium/src/services/file/manifest.json?g=0)

1. service启动，调用`registry_.AddInterface`

2. other service，创建`file::mojom::FileSystemPtr`， 调用`connector_->BindInterface`， 使用mojom interface：`file_system_->GetSubDirectory`

3. service， `FileService::OnBindInterface`被调用，调用`registry_.BindInterface`，这样就bind好了？


层级：

1. other service

2. public mojo interface, service manager, service

3. service

4. mojo private implement


service manager的内部实现还没看，这里好像是看懂怎么用service了。

昨天收到一个cr，看到NG代码，脑袋马上清楚了许多。而service看了这么久，还跟浆糊似的，哎。。。





