#include "ClientHangarModule.hpp"
#include <memory>
#include <string>
#include <vector>
#include "ClientConstants.hpp"
#include "ConnectionPhase.hpp"
#include "ConnectionStatus.hpp"
#include "HangarScreen.hpp"
#include "HangarScreenIdConstants.hpp"
#include "HangarScreenParts.hpp"
#include "IScreen.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "PilotColor.hpp"
#include "PlayerPreferences.hpp"
#include "ProtocolConstants.hpp"
#include "ScreenBuilding.hpp"
#include "ScreenContext.hpp"
#include "ScreenRegistry.hpp"
#include "SettingsService.hpp"
#include "ShipLookExchange.hpp"
#include "TimeConstants.hpp"
#include "WonItems.hpp"

namespace rtype::client {

namespace {

/** @returns The pilot color of the player once a server accepted them, the
 * color of the first player before. */
int shipColorOf(ConnectionStatus status) {
  return pilotColorOfPlayerNumber(status.phase == ConnectionPhase::Accepted
                                      ? status.playerNumber
                                      : network::FIRST_PLAYER_NUMBER);
}

std::unique_ptr<IScreen> buildHangarScreen(const ScreenContext& context) {
  return std::make_unique<HangarScreen>(
      HangarScreenParts{
          .navigator = context.navigator.get(),
          .sounds = soundsOf(context),
          .audio = context.services.audio.get(),
          .settings = context.services.settingsService.get(),
          .playfieldPointAt = playfieldMappingOf(context),
          .palette = playerPaletteOf(context.services.settings.get()),
          .shipColor = shipColorOf(context.services.connection.get().status()),
          .motion = motionPreference()},
      interfaceCanvasOf(context));
}

}  // namespace

std::string ClientHangarModule::name() const {
  return std::string{HANGAR_MODULE_NAME};
}

std::vector<std::string> ClientHangarModule::dependencies() const {
  return {std::string{SCREENS_MODULE_NAME}, std::string{CONNECTION_MODULE_NAME},
          std::string{SETTINGS_MODULE_NAME}};
}

void ClientHangarModule::install(engine::InstallContext& context) {
  context.require<ScreenRegistry>().add(HANGAR_SCREEN, buildHangarScreen);
  shipLooks_ = &context.require<ShipLookExchange>();
  settings_ = &context.require<SettingsService>();
  context.addWork(UPDATE_STAGE,
                  [this]([[maybe_unused]] engine::Duration elapsed) {
                    storeWonItems(*shipLooks_, *settings_);
                  });
}

void ClientHangarModule::start() {}

void ClientHangarModule::stop() {}

}  // namespace rtype::client
