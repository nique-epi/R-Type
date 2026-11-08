#include "ClientLobbyModule.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "GameRules.hpp"
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
#include "ServerChoice.hpp"

namespace rtype::client {

namespace {

/** @returns The rules of the game of the last attempt, when the client knows
 * them. */
std::optional<network::GameRules> rulesOfLastAttempt(
    const ServerChoice& choice) {
  if (!choice.lastRequest.has_value()) {
    return std::nullopt;
  }
  return choice.lastRequest->rules;
}

/** @returns The waiting room, its READY key read from the bindings of the
 * player when it is built. */
std::unique_ptr<IScreen> buildLobbyScreen(const ScreenContext& context,
                                          LobbyPlace place) {
  const PlayerSettings& settings = context.services.settings.get();
  return std::make_unique<LobbyScreen>(
      LobbyScreenParts{.navigator = context.navigator.get(),
                       .sounds = soundsOf(context),
                       .connection = context.services.connection.get(),
                       .waitingRoom = context.services.waitingRoom.get(),
                       .shipLooks = context.services.shipLooks.get(),
                       .playfieldPointAt = playfieldMappingOf(context),
                       .keys = settings.keys,
                       .keyName = localizedKeyName,
                       .palette = playerPaletteOf(settings),
                       .look = settings.shipLook,
                       .motion = motionPreference(),
                       .place = place,
                       .chat = context.services.chat.get(),
                       .settings = settings,
                       .rules = rulesOfLastAttempt(context.serverChoice.get())},
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
  return {std::string{SCREENS_MODULE_NAME}};
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
