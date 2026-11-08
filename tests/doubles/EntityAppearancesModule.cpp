#include "EntityAppearancesModule.hpp"
#include <span>
#include <string>
#include <vector>
#include "EntityAppearances.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "SheetText.hpp"

EntityAppearancesModule::EntityAppearancesModule()
    : appearances_(rtype::client::EntityAppearances::fromTexts(
          std::span<const rtype::game::SheetText>{})) {}

std::string EntityAppearancesModule::name() const {
  return std::string{rtype::client::ASSETS_MODULE_NAME};
}

std::vector<std::string> EntityAppearancesModule::dependencies() const {
  return {};
}

void EntityAppearancesModule::install(rtype::engine::InstallContext& context) {
  context.provide(appearances_);
}

void EntityAppearancesModule::start() {}

void EntityAppearancesModule::stop() {}
