#include "ClientCasinoModule.hpp"
#include <SFML/Graphics/Image.hpp>
#include <memory>
#include <string>
#include <vector>
#include "CashierScreen.hpp"
#include "CashierScreenParts.hpp"
#include "CashierWay.hpp"
#include "CasinoEntranceScreen.hpp"
#include "CasinoEntranceScreenParts.hpp"
#include "CasinoHallScreen.hpp"
#include "CasinoHallScreenParts.hpp"
#include "CasinoScreenIdConstants.hpp"
#include "CasinoSpriteConstants.hpp"
#include "ClientConstants.hpp"
#include "HallPictureScan.hpp"
#include "IAccountSession.hpp"
#include "ICasinoChannel.hpp"
#include "ICasinoLink.hpp"
#include "IScreen.hpp"
#include "IWallet.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "PlayerPreferences.hpp"
#include "PlayerSettings.hpp"
#include "RouletteMemory.hpp"
#include "RouletteScreen.hpp"
#include "RouletteScreenParts.hpp"
#include "ScreenBuilding.hpp"
#include "ScreenContext.hpp"
#include "ScreenRegistry.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The casino, its doors then its hall, for a player signed in
 * only. */
std::unique_ptr<IScreen> buildCasinoScreen(const ScreenContext& context,
                                           ICasinoChannel& casino) {
  return std::make_unique<CasinoEntranceScreen>(
      CasinoEntranceScreenParts{.navigator = context.navigator.get(),
                                .sounds = soundsOf(context),
                                .casino = casino,
                                .wallet = context.services.wallet.get(),
                                .playfieldPointAt = playfieldMappingOf(context),
                                .motion = motionPreference()},
      interfaceCanvasOf(context));
}

/** @returns The picture of @p assetId, read from its file in the assets of
 * @p context; an empty one when it cannot be read, and the room that reads
 * it stays still. */
sf::Image pictureOf(const ScreenContext& context, const char* assetId) {
  sf::Image picture;
  if (!picture.loadFromFile(context.services.assets.get().fileOf(assetId))) {
    return sf::Image{};
  }
  return picture;
}

/** @returns The main hall of the casino, its lights read in its picture. */
std::unique_ptr<IScreen> buildHallScreen(const ScreenContext& context,
                                         ICasinoChannel& casino) {
  return std::make_unique<CasinoHallScreen>(
      CasinoHallScreenParts{
          .navigator = context.navigator.get(),
          .sounds = soundsOf(context),
          .casino = casino,
          .playfieldPointAt = playfieldMappingOf(context),
          .picture = hallPictureOf(pictureOf(context, HALL_PICTURE_ID)),
          .motion = motionPreference()},
      interfaceCanvasOf(context));
}

/** @returns The cashier, changing kopecks and chips @p way, for a player
 * signed in only. */
std::unique_ptr<IScreen> buildCashierScreen(const ScreenContext& context,
                                            CashierWay way) {
  return std::make_unique<CashierScreen>(
      CashierScreenParts{.navigator = context.navigator.get(),
                         .sounds = soundsOf(context),
                         .wallet = context.services.wallet.get(),
                         .playfieldPointAt = playfieldMappingOf(context),
                         .way = way},
      interfaceCanvasOf(context));
}

/** @returns The roulette table, its bets kept in @p memory. */
std::unique_ptr<IScreen> buildRouletteScreen(const ScreenContext& context,
                                             ICasinoChannel& casino,
                                             RouletteMemory& memory) {
  const PlayerSettings& settings = context.services.settings.get();
  return std::make_unique<RouletteScreen>(
      RouletteScreenParts{.navigator = context.navigator.get(),
                          .sounds = soundsOf(context),
                          .casino = casino,
                          .memory = memory,
                          .wallet = context.services.wallet.get(),
                          .account = context.services.account.get(),
                          .playfieldPointAt = playfieldMappingOf(context),
                          .palette = playerPaletteOf(settings),
                          .motion = motionPreference()},
      interfaceCanvasOf(context));
}

}  // namespace

std::string ClientCasinoModule::name() const {
  return std::string{CASINO_MODULE_NAME};
}

std::vector<std::string> ClientCasinoModule::dependencies() const {
  return {std::string{SCREENS_MODULE_NAME}, std::string{CONNECTION_MODULE_NAME},
          std::string{ACCOUNT_MODULE_NAME}};
}

void ClientCasinoModule::install(engine::InstallContext& context) {
  casino_.emplace(context.require<ICasinoLink>(),
                  context.require<IAccountSession>(),
                  context.require<IWallet>());
  context.require<ScreenRegistry>().add(
      CASINO_SCREEN, signedInOnly(
                         [this](const ScreenContext& screens) {
                           return buildCasinoScreen(screens, *casino_);
                         },
                         CASINO_SCREEN));
  context.require<ScreenRegistry>().add(
      CASINO_HALL_SCREEN, signedInOnly(
                              [this](const ScreenContext& screens) {
                                return buildHallScreen(screens, *casino_);
                              },
                              CASINO_HALL_SCREEN));
  context.require<ScreenRegistry>().add(
      CASINO_BUY_CHIPS_SCREEN, signedInOnly(
                                   [](const ScreenContext& screens) {
                                     return buildCashierScreen(
                                         screens, CashierWay::BuyChips);
                                   },
                                   CASINO_BUY_CHIPS_SCREEN));
  context.require<ScreenRegistry>().add(CASINO_CASH_OUT_SCREEN,
                                        signedInOnly(
                                            [](const ScreenContext& screens) {
                                              return buildCashierScreen(
                                                  screens, CashierWay::CashOut);
                                            },
                                            CASINO_CASH_OUT_SCREEN));
  context.require<ScreenRegistry>().add(
      ROULETTE_SCREEN, [this](const ScreenContext& screens) {
        return buildRouletteScreen(screens, *casino_, roulette_);
      });
  context.addWork(UPDATE_STAGE, [this](engine::Duration elapsed) {
    casino_->update(elapsed);
    roulette_.follow(casino_->room(), casino_->lastBets());
  });
}

void ClientCasinoModule::start() {}

void ClientCasinoModule::stop() {}

}  // namespace rtype::client
