# mojo and services

如何创建mojo？

## mojo

1. 创建logger.mojom，以及对应的BUILD.gn
2. logger的实现：`class Logger : public mojom::Logger`

## services

1. 创建service: `CctestService : public service_manager::Service`。class之中包含：`registry_`

  - 记得AddInterface

```
CctestService::CctestService() {
  LOG(INFO) << "CctestService::CctestService";
  registry_.AddInterface<mojom::Logger>(base::Bind(&OnBindLogger));
}
```

  - 实现OnBindLogger
  - 实现OnBindInterface

```
void CctestService::OnBindInterface(const service_manager::BindSourceInfo& remote_info,
                                const std::string& interface_name,
                                mojo::ScopedMessagePipeHandle handle) {
  LOG(INFO) << "CctestService::OnBindInterface";
  registry_.BindInterface(interface_name, std::move(handle), remote_info);
}
```


2. 创建manifest.json和BUILD.gn


## 如何使用？

1. `BrowserContext::Initialize`时添加cctest service

```
    {
      service_manager::EmbeddedServiceInfo info;
      info.factory = base::BindRepeating(&cctest::CreateCctestService);
      connection->AddEmbeddedService(cctest::mojom::kServiceName, info);
    }
```

2. content/browser/BUILD.gn中添加deps

`"//services/cctest:lib",`

3. 需要使用到mojo的service的manifest.jason中添加相应的require

content/public/app/BUILD.gn

`service_manifest("renderer_manifest")` 添加

```
  packaged_services = [
    "//services/cctest:manifest",
  ]

```

4. 使用mojo的地方的BUILD.gn添加mojo的deps
如：在`LocalFrameView::UpdateLayout`中使用mojo interface，那么就应该在frame目录下的BUILD.gn添加deps

```
  deps = [
    "//services/cctest/public:mojom",
  ]

```


使用的代码如下：

```
    cctest::mojom::LoggerPtr logger;
    Platform::Current()->GetConnector()->BindInterface(cctest::mojom::kServiceName, &logger);
    logger->Log("I must call a thousand times...");

```
