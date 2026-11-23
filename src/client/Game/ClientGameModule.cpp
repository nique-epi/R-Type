#include "ClientGameModule.hpp"
#include <memory>
#include <random>
#include <string>
#include <vector>
#include "AssetList.hpp"
#include "EndScreen.hpp"
#include "EndScreenParts.hpp"
#include "EntityLayer.hpp"
#include "GameScreen.hpp"
#include "GameScreenIdConstants.hpp"
#include "GameScreenParts.hpp"
#include "IScreen.hpp"
#include "ISoundPlayer.hpp"
#include "InstallContext.hpp"
#include "InterpolationTally.hpp"
#include "ModuleConstants.hpp"
#include "OpeningSky.hpp"
#include "PlayerPreferences.hpp"
#include "PlayerSettings.hpp"
#include "ScreenAssets.hpp"
#include "ScreenBuilding.hpp"
#include "ScreenContext.hpp"
#include "ScreenRegistry.hpp"
#include "ServerSession.hpp"

namespace rtype::client {

namespace {

/** @returns The assets of a game, with the sprite sheet of every entity
 * appearance of the content of the session (see withEntitySheets()) and the
 * images of the backgrounds its pack brings (see withBackgroundImages()). */
AssetList gameAssetsOf(const ScreenContext& context) {
  return withBackgroundImages(
      withEntitySheets(gameAssets(),
                       context.services.entityTable.get().appearances()),
      context.services.sessionPack.get().backgrounds().get());
}

/** @brief Reads every sound the sheets of the content of the session name,
 * so the game plays them without reading the disk; a sound read before is
 * not read again. */
void loadEntitySounds(const ScreenContext& context) {
  ISoundPlayer& sounds = soundsOf(context);
  for (const std::string& sound :
       context.services.entityTable.get().appearances().sounds()) {
    sounds.load(sound);
  }
}

/** @returns The loading screen of gameAssetsOf() while some are not loaded,
 * the game screen once they all are, which counts in @p tally how it shows
 * the other entities; both over the sky the level opens in (see
 * openTheLevel()). */
std::unique_ptr<IScreen> buildGameScreen(const ScreenContext& context,
                                         InterpolationTally& tally) {
  openTheLevel(context.services.announcedStart.get(), context.sky.get(),
               context.services.audio.get());
  loadEntitySounds(context);
  std::unique_ptr<IScreen> loading =
      loadingScreenUnlessLoaded(context, gameAssetsOf(context), GAME_SCREEN);
  if (loading != nullptr) {
    return loading;
  }
  ServerSession& connection = context.services.connection.get();
  return std::make_unique<GameScreen>(
      GameScreenParts{.navigator = context.navigator.get(),
                      .settings = context.services.settings.get(),
                      .inputs = connection,
                      .entityMessages = context.services.entityMessages.get(),
                      .serverTicks = context.services.serverTicks.get(),
                      .connection = connection,
                      .waitingRoom = context.services.waitingRoom.get(),
                      .shipLooks = context.services.shipLooks.get(),
                      .sky = context.sky.get(),
                      .sounds = soundsOf(context),
                      .audio = context.services.audio.get(),
                      .entityTypes = context.services.entityTypes.get(),
                      .result = context.gameResult.get(),
                      .chat = context.services.chat.get(),
                      .interpolationTally = tally},
      std::make_unique<EntityLayer>(context.services.assets.get()),
      interfaceCanvasOf(context));
}

/** @returns The loading screen of gameAssetsOf() while some are not loaded,
 * the end screen once they all are. */
std::unique_ptr<IScreen> buildEndScreen(const ScreenContext& context) {
  std::unique_ptr<IScreen> loading =
      loadingScreenUnlessLoaded(context, gameAssetsOf(context), END_SCREEN);
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
  return {std::string{SCREENS_MODULE_NAME}};
}

void ClientGameModule::install(engine::InstallContext& context) {
  auto& registry = context.require<ScreenRegistry>();
  context.provide(interpolationTally_);
  registry.add(GAME_SCREEN, [this](const ScreenContext& screenContext) {
    return buildGameScreen(screenContext, interpolationTally_);
  });
  registry.add(END_SCREEN, buildEndScreen);
}

void ClientGameModule::start() {}

void ClientGameModule::stop() {}

}  // namespace rtype::client
