#include "ScreenRegistryProbeModule.hpp"
#include <string>
#include <vector>
#include "InstallContext.hpp"
#include "ScreenRegistry.hpp"

ScreenRegistryProbeModule::ScreenRegistryProbeModule(
    rtype::client::ScreenRegistry*& registry)
    : registry_(&registry) {}

std::string ScreenRegistryProbeModule::name() const {
  return "screen-registry-probe";
}

std::vector<std::string> ScreenRegistryProbeModule::dependencies() const {
  return {};
}

void ScreenRegistryProbeModule::install(
    rtype::engine::InstallContext& context) {
  *registry_ = &context.require<rtype::client::ScreenRegistry>();
}

void ScreenRegistryProbeModule::start() {}

void ScreenRegistryProbeModule::stop() {}
