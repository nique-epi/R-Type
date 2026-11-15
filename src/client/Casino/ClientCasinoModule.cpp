#include "ClientCasinoModule.hpp"
#include <memory>
#include <string>
#include <vector>
#include "CashierScreen.hpp"
#include "CashierScreenParts.hpp"
#include "CashierWay.hpp"
#include "CasinoEntranceScreen.hpp"
#include "CasinoEntranceScreenParts.hpp"
#include "CasinoScreenIdConstants.hpp"
#include "ChipSettlement.hpp"
#include "ClientConstants.hpp"
#include "ICasinoChannel.hpp"
#include "IScreen.hpp"
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
#include "SettingsService.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

std::unique_ptr<IScreen> buildCasinoScreen(const ScreenContext& context,
                                           ICasinoChannel& casino) {
  return std::make_unique<CasinoEntranceScreen>(
      CasinoEntranceScreenParts{.navigator = context.navigator.get(),
                                .sounds = soundsOf(context),
                                .casino = casino,
                                .settings = context.services.settings.get(),
                                .playfieldPointAt = playfieldMappingOf(context),
                                .motion = motionPreference()},
      interfaceCanvasOf(context));
}

/** @returns The cashier, changing kopecks and chips @p way. */
std::unique_ptr<IScreen> buildCashierScreen(const ScreenContext& context,
                                            CashierWay way) {
  return std::make_unique<CashierScreen>(
      CashierScreenParts{.navigator = context.navigator.get(),
                         .sounds = soundsOf(context),
                         .settings = context.services.settingsService.get(),
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
                          .settings = settings,
                          .connection = context.services.connection.get(),
                          .waitingRoom = context.services.waitingRoom.get(),
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
          std::string{SETTINGS_MODULE_NAME}};
}

void ClientCasinoModule::install(engine::InstallContext& context) {
  casino_ = &context.require<ICasinoChannel>();
  settings_ = &context.require<SettingsService>();
  context.require<ScreenRegistry>().add(
      CASINO_SCREEN, [casino = casino_](const ScreenContext& screens) {
        return buildCasinoScreen(screens, *casino);
      });
  context.require<ScreenRegistry>().add(
      CASINO_BUY_CHIPS_SCREEN, [](const ScreenContext& screens) {
        return buildCashierScreen(screens, CashierWay::BuyChips);
      });
  context.require<ScreenRegistry>().add(
      CASINO_CASH_OUT_SCREEN, [](const ScreenContext& screens) {
        return buildCashierScreen(screens, CashierWay::CashOut);
      });
  context.require<ScreenRegistry>().add(
      ROULETTE_SCREEN, [this](const ScreenContext& screens) {
        return buildRouletteScreen(screens, *casino_, roulette_);
      });
  context.addWork(
      UPDATE_STAGE, [this]([[maybe_unused]] engine::Duration elapsed) {
        roulette_.follow(casino_->round(), settings_->settings().chips);
        settleChipMoves(*casino_, *settings_);
      });
}

void ClientCasinoModule::start() {}

void ClientCasinoModule::stop() {}

}  // namespace rtype::client
