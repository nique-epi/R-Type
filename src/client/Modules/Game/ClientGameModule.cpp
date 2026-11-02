#include "ClientGameModule.hpp"
#include <memory>
#include <random>
#include <string>
#include <vector>
#include "EndScreen.hpp"
#include "EndScreenParts.hpp"
#include "EntityLayer.hpp"
#include "GameScreen.hpp"
#include "GameScreenIdConstants.hpp"
#include "GameScreenParts.hpp"
#include "IScreen.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "PlayerPreferences.hpp"
#include "PlayerSettings.hpp"
#include "ScreenAssets.hpp"
#include "ScreenBuilding.hpp"
#include "ScreenContext.hpp"
#include "ScreenRegistry.hpp"
#include "ServerConnection.hpp"

namespace rtype::client {

namespace {

/** @returns The loading screen of gameAssets() while some are not loaded, the
 * game screen once they all are. */
std::unique_ptr<IScreen> buildGameScreen(const ScreenContext& context) {
  context.services.audio.get().playGameMusic();
  std::unique_ptr<IScreen> loading =
      loadingScreenUnlessLoaded(context, gameAssets(), GAME_SCREEN);
  if (loading != nullptr) {
    return loading;
  }
  ServerConnection& connection = context.services.connection.get();
  return std::make_unique<GameScreen>(
      GameScreenParts{.navigator = context.navigator.get(),
                      .settings = context.services.settings.get(),
                      .inputs = connection,
                      .entityMessages = connection,
                      .connection = connection,
                      .sky = context.sky.get(),
                      .sounds = soundsOf(context),
                      .result = context.gameResult.get()},
      std::make_unique<EntityLayer>(context.services.assets.get()),
      interfaceCanvasOf(context));
}

/** @returns The loading screen of gameAssets() while some are not loaded, the
 * end screen once they all are. */
std::unique_ptr<IScreen> buildEndScreen(const ScreenContext& context) {
  std::unique_ptr<IScreen> loading =
      loadingScreenUnlessLoaded(context, gameAssets(), END_SCREEN);
  if (loading != nullptr) {
    return loading;
  }
  const PlayerSettings& settings = context.services.settings.get();
  return std::make_unique<EndScreen>(
      EndScreenParts{.navigator = context.navigator.get(),
                     .sounds = soundsOf(context),
                     .connection = context.services.connection.get(),
                     .result = context.gameResult.get(),
                     .previousTaunt = context.previousTaunt.get(),
                     .playfieldPointAt = playfieldMappingOf(context),
                     .seed = std::random_device{}(),
                     .motion = motionPreference(),
                     .flashes = flashPreferenceOf(settings),
                     .palette = playerPaletteOf(settings)},
      interfaceCanvasOf(context));
}

}  // namespace

std::string ClientGameModule::name() const {
  return std::string{GAME_MODULE_NAME};
}

std::vector<std::string> ClientGameModule::dependencies() const {
  return {std::string{SCREEN_REGISTRY_MODULE_NAME}};
}

void ClientGameModule::install(engine::InstallContext& context) {
  auto& registry = context.require<ScreenRegistry>();
  registry.add(GAME_SCREEN, buildGameScreen);
  registry.add(END_SCREEN, buildEndScreen);
}

void ClientGameModule::start() {}

void ClientGameModule::stop() {}

}  // namespace rtype::client
