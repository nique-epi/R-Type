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

/** @returns The shop for a player signed in; the dialog that says a sign-in
 * is needed for a guest, since the kopecks live on the account. */
std::unique_ptr<IScreen> buildShopScreen(const ScreenContext& context) {
  if (!context.services.account.get().signedIn()) {
    return signInNeededScreenOf(context);
  }
  return std::make_unique<ShopScreen>(
      ShopScreenParts{.navigator = context.navigator.get(),
                      .sounds = soundsOf(context),
                      .audio = context.services.audio.get(),
                      .wallet = context.services.wallet.get(),
                      .playfieldPointAt = playfieldMappingOf(context),
                      .motion = motionPreference()},
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
