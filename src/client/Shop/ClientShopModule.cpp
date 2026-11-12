#include "ClientShopModule.hpp"
#include <memory>
#include <string>
#include <vector>
#include "IScreen.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "ScreenBuilding.hpp"
#include "ScreenContext.hpp"
#include "ScreenRegistry.hpp"
#include "ShopScreen.hpp"
#include "ShopScreenIdConstants.hpp"
#include "ShopScreenParts.hpp"

namespace rtype::client {

namespace {

std::unique_ptr<IScreen> buildShopScreen(const ScreenContext& context) {
  return std::make_unique<ShopScreen>(
      ShopScreenParts{.navigator = context.navigator.get(),
                      .sounds = soundsOf(context),
                      .settings = context.services.settingsService.get(),
                      .playfieldPointAt = playfieldMappingOf(context)},
      interfaceCanvasOf(context));
}

}  // namespace

std::string ClientShopModule::name() const {
  return std::string{SHOP_MODULE_NAME};
}

std::vector<std::string> ClientShopModule::dependencies() const {
  return {std::string{SCREENS_MODULE_NAME}};
}

void ClientShopModule::install(engine::InstallContext& context) {
  context.require<ScreenRegistry>().add(SHOP_SCREEN, buildShopScreen);
}

void ClientShopModule::start() {}

void ClientShopModule::stop() {}

}  // namespace rtype::client
