#include "services/cctest/cctest_service.h"
#include "services/service_manager/public/c/main.h"
#include "services/service_manager/public/cpp/service_runner.h"

MojoResult ServiceMain(MojoHandle service_request_handle) {
  return service_manager::ServiceRunner(new cctest::CctestService).Run(
      service_request_handle);
}
