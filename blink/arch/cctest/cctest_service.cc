#include "services/cctest/cctest_service.h"
#include "services/cctest/logger.h"
#include "mojo/public/cpp/bindings/strong_binding.h"
#include "services/service_manager/public/cpp/bind_source_info.h"


namespace {

void OnBindLogger(cctest::mojom::LoggerRequest request, const service_manager::BindSourceInfo& info) {
  LOG(INFO) << "OnBindLogger";
  mojo::MakeStrongBinding(std::make_unique<cctest::Logger>(),
                        std::move(request));
}

}


namespace cctest {

std::unique_ptr<service_manager::Service> CreateCctestService() {
  return std::make_unique<CctestService>();
}

CctestService::CctestService() {
  LOG(INFO) << "CctestService::CctestService";
  registry_.AddInterface<mojom::Logger>(base::Bind(&OnBindLogger));
}

CctestService::~CctestService() = default;

void CctestService::OnStart() {
  LOG(INFO) << "CctestService::OnStart";
}

void CctestService::OnBindInterface(const service_manager::BindSourceInfo& remote_info,
                                const std::string& interface_name,
                                mojo::ScopedMessagePipeHandle handle) {
  LOG(INFO) << "CctestService::OnBindInterface";
  registry_.BindInterface(interface_name, std::move(handle), remote_info);
}

}  // namespace cctest
