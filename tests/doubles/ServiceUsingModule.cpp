#include "ServiceUsingModule.hpp"
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "InstallContext.hpp"
#include "JournalModule.hpp"
#include "ServiceUsingModuleConstants.hpp"
#include "ServiceUsingModuleSetup.hpp"
#include "TestService.hpp"

ServiceUsingModule::ServiceUsingModule(ServiceUsingModuleSetup setup,
                                       JournalModule& journal)
    : setup_(std::move(setup)), journal_(&journal) {}

std::string ServiceUsingModule::name() const { return setup_.name; }

std::vector<std::string> ServiceUsingModule::dependencies() const { return {}; }

void ServiceUsingModule::install(rtype::engine::InstallContext& context) {
  record(foundService(context) ? SERVICE_FOUND_CALL : SERVICE_ABSENT_CALL);
}

void ServiceUsingModule::start() {}

void ServiceUsingModule::stop() {}

bool ServiceUsingModule::foundService(
    const rtype::engine::InstallContext& context) const {
  if (setup_.requiresService) {
    [[maybe_unused]] const TestService& service =
        context.require<TestService>();
    return true;
  }
  return context.find<TestService>() != nullptr;
}

void ServiceUsingModule::record(std::string_view call) {
  journal_->calls.push_back(std::string(call) + setup_.name);
}
