#include "base/macros.h"
#include "services/service_manager/public/cpp/binder_registry.h"
#include "services/service_manager/public/cpp/service.h"
#include "services/cctest/public/logger.mojom.h"


namespace cctest {

std::unique_ptr<service_manager::Service> CreateCctestService();

class CctestService : public service_manager::Service {
 public:
  CctestService();
  ~CctestService() override;

  // service_manager::Service:
  void OnStart() override;
  void OnBindInterface(const service_manager::BindSourceInfo& source_info,
                       const std::string& interface_name,
                       mojo::ScopedMessagePipeHandle handle) override;
 private:
  service_manager::BinderRegistryWithArgs<const service_manager::BindSourceInfo&> registry_;

  DISALLOW_COPY_AND_ASSIGN(CctestService);
};

}  // namespace cctest
