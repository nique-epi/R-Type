#include "ScreenRegistryProvidingModule.hpp"
#include <string>
#include <vector>
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "ScreenRegistry.hpp"

ScreenRegistryProvidingModule::ScreenRegistryProvidingModule(
    rtype::client::ScreenRegistry& registry)
    : registry_(&registry) {}

std::string ScreenRegistryProvidingModule::name() const {
  return std::string{rtype::client::SCREENS_MODULE_NAME};
}

std::vector<std::string> ScreenRegistryProvidingModule::dependencies() const {
  return {};
}

void ScreenRegistryProvidingModule::install(
    rtype::engine::InstallContext& context) {
  context.provide(*registry_);
}

void ScreenRegistryProvidingModule::start() {}

void ScreenRegistryProvidingModule::stop() {}
