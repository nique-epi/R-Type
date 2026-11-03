#include "ScreenModulesFixture.hpp"
#include <memory>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "ClientException.hpp"
#include "IModule.hpp"
#include "ScreenContext.hpp"
#include "ScreenId.hpp"
#include "ScreenRegistryProvidingModule.hpp"

using rtype::client::DuplicateScreenException;
using rtype::client::ScreenId;

void ScreenModulesFixture::install(
    std::vector<std::unique_ptr<rtype::engine::IModule>> screenModules) {
  screenModules.insert(
      screenModules.begin(),
      std::make_unique<ScreenRegistryProvidingModule>(registry_));
  application_ =
      std::make_unique<rtype::engine::Application>(std::move(screenModules));
  application_->start();
}

bool ScreenModulesFixture::isRegistered(ScreenId screen) {
  try {
    registry_.add(screen,
                  [](const rtype::client::ScreenContext&) { return nullptr; });
  } catch (const DuplicateScreenException&) {
    return true;
  }
  return false;
}
