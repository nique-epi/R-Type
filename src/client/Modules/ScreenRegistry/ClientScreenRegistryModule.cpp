#include "ClientScreenRegistryModule.hpp"
#include <string>
#include <vector>
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"

namespace rtype::client {

std::string ClientScreenRegistryModule::name() const {
  return std::string{SCREEN_REGISTRY_MODULE_NAME};
}

std::vector<std::string> ClientScreenRegistryModule::dependencies() const {
  return {};
}

void ClientScreenRegistryModule::install(engine::InstallContext& context) {
  context.provide(registry_);
}

void ClientScreenRegistryModule::start() {}

void ClientScreenRegistryModule::stop() {}

}  // namespace rtype::client
