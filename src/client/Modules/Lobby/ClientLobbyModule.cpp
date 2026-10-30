#include "ClientLobbyModule.hpp"
#include <memory>
#include <string>
#include <vector>
#include "IScreen.hpp"
#include "InstallContext.hpp"
#include "LobbyPlace.hpp"
#include "LobbyScreen.hpp"
#include "LobbyScreenIdConstants.hpp"
#include "LobbyScreenParts.hpp"
#include "LocalizedKeyName.hpp"
#include "ModuleConstants.hpp"
#include "PlayerPreferences.hpp"
#include "PlayerSettings.hpp"
#include "ScreenBuilding.hpp"
#include "ScreenContext.hpp"
#include "ScreenRegistry.hpp"

namespace rtype::client {

namespace {

/** @returns The waiting room, its READY key read from the bindings of the
 * player when it is built. */
std::unique_ptr<IScreen> buildLobbyScreen(const ScreenContext& context,
                                          LobbyPlace place) {
  const PlayerSettings& settings = context.services.settings.get();
  return std::make_unique<LobbyScreen>(
      LobbyScreenParts{.navigator = context.navigator.get(),
                       .sounds = soundsOf(context),
                       .connection = context.services.connection.get(),
                       .playfieldPointAt = playfieldMappingOf(context),
                       .keys = settings.keys,
                       .keyName = localizedKeyName,
                       .palette = playerPaletteOf(settings),
                       .motion = motionPreference(),
                       .place = place},
      interfaceCanvasOf(context));
}

/** @returns The waiting room alone after a game, playing the theme again. */
std::unique_ptr<IScreen> buildLobbyAfterGameScreen(
    const ScreenContext& context) {
  context.services.audio.get().playTheme();
  return buildLobbyScreen(context, LobbyPlace::AfterGame);
}

}  // namespace

std::string ClientLobbyModule::name() const {
  return std::string{LOBBY_MODULE_NAME};
}

std::vector<std::string> ClientLobbyModule::dependencies() const {
  return {std::string{SCREEN_REGISTRY_MODULE_NAME}};
}

void ClientLobbyModule::install(engine::InstallContext& context) {
  auto& registry = context.require<ScreenRegistry>();
  registry.add(LOBBY_SCREEN, [](const ScreenContext& screens) {
    return buildLobbyScreen(screens, LobbyPlace::AfterConnection);
  });
  registry.add(LOBBY_AFTER_GAME_SCREEN, buildLobbyAfterGameScreen);
}

void ClientLobbyModule::start() {}

void ClientLobbyModule::stop() {}

}  // namespace rtype::client
