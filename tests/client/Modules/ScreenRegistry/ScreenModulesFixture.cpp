#include "ScreenModulesFixture.hpp"
#include <memory>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "ClientException.hpp"
#include "ClientScreenRegistryModule.hpp"
#include "IModule.hpp"
#include "ScreenContext.hpp"
#include "ScreenId.hpp"
#include "ScreenRegistry.hpp"
#include "ScreenRegistryProbeModule.hpp"

using rtype::client::ClientScreenRegistryModule;
using rtype::client::DuplicateScreenException;
using rtype::client::ScreenId;

void ScreenModulesFixture::install(
    std::vector<std::unique_ptr<rtype::engine::IModule>> screenModules) {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(std::make_unique<ClientScreenRegistryModule>());
  for (auto& module : screenModules) {
    modules.push_back(std::move(module));
  }
  modules.push_back(std::make_unique<ScreenRegistryProbeModule>(registry_));
  application_ =
      std::make_unique<rtype::engine::Application>(std::move(modules));
  application_->start();
}

bool ScreenModulesFixture::isRegistered(ScreenId screen) {
  try {
    registry_->add(screen,
                   [](const rtype::client::ScreenContext&) { return nullptr; });
  } catch (const DuplicateScreenException&) {
    return true;
  }
  return false;
}
