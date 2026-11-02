#include "ClientMenusModule.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <random>
#include <string>
#include <vector>
#include "ConnectionScreen.hpp"
#include "ConnectionScreenParts.hpp"
#include "DialogMenus.hpp"
#include "IScreen.hpp"
#include "InstallContext.hpp"
#include "IntroScreen.hpp"
#include "IntroScreenParts.hpp"
#include "LeaderboardScreen.hpp"
#include "LeaderboardScreenConstants.hpp"
#include "LeaderboardScreenParts.hpp"
#include "LeaderboardTab.hpp"
#include "LocalizedKeyName.hpp"
#include "MenuScreenIdConstants.hpp"
#include "ModuleConstants.hpp"
#include "OptionsPlace.hpp"
#include "OptionsScreen.hpp"
#include "OptionsScreenParts.hpp"
#include "ScreenAssets.hpp"
#include "ScreenBuilding.hpp"
#include "ScreenContext.hpp"
#include "ScreenId.hpp"
#include "ScreenRegistry.hpp"
#include "SquadronStart.hpp"
#include "TitleScreen.hpp"
#include "TitleScreenParts.hpp"

namespace rtype::client {

namespace {

std::unique_ptr<IScreen> buildIntroScreen(const ScreenContext& context) {
  return std::make_unique<IntroScreen>(
      IntroScreenParts{.navigator = context.navigator.get(),
                       .settings = context.services.settings.get(),
                       .sky = context.sky.get(),
                       .sounds = soundsOf(context),
                       .music = context.services.audio.get().theme(),
                       .seed = std::random_device{}()},
      interfaceCanvasOf(context));
}

/** @returns The loading screen of titleAssets() while some are not loaded,
 * the title screen once they all are; its squadron is still when the system
 * asks for less motion, and starts as @p squadronStart says otherwise. */
std::unique_ptr<IScreen> buildTitleScreen(const ScreenContext& context,
                                          ScreenId screen,
                                          SquadronStart squadronStart) {
  context.services.audio.get().playTheme();
  std::unique_ptr<IScreen> loading =
      loadingScreenUnlessLoaded(context, titleAssets(), screen);
  if (loading != nullptr) {
    return loading;
  }
  return std::make_unique<TitleScreen>(
      TitleScreenParts{.navigator = context.navigator.get(),
                       .sounds = soundsOf(context),
                       .settings = context.services.settings.get(),
                       .playfieldPointAt = playfieldMappingOf(context),
                       .motion = motionPreference(),
                       .seed = std::random_device{}(),
                       .squadronStart = squadronStart},
      interfaceCanvasOf(context));
}

std::unique_ptr<IScreen> buildOptionsScreen(const ScreenContext& context,
                                            OptionsPlace place) {
  return std::make_unique<OptionsScreen>(
      OptionsScreenParts{.navigator = context.navigator.get(),
                         .sounds = soundsOf(context),
                         .place = place,
                         .settings = context.services.settingsService.get(),
                         .window = context.services.window.get(),
                         .keyName = localizedKeyName,
                         .playfieldPointAt = playfieldMappingOf(context)},
      interfaceCanvasOf(context));
}

std::unique_ptr<IScreen> buildQuitConfirmationScreen(
    const ScreenContext& context) {
  return dialogScreenOf(context, quitConfirmationMenu(context.navigator.get(),
                                                      soundsOf(context)));
}

std::unique_ptr<IScreen> buildConnectionLostScreen(
    const ScreenContext& context) {
  return dialogScreenOf(
      context,
      connectionLostMenu(context.navigator.get(), soundsOf(context),
                         context.services.connection.get().status().phase));
}

std::unique_ptr<IScreen> buildConnectionScreen(const ScreenContext& context) {
  return std::make_unique<ConnectionScreen>(
      ConnectionScreenParts{.navigator = context.navigator.get(),
                            .sounds = soundsOf(context),
                            .connection = context.services.connection.get(),
                            .playfieldPointAt = playfieldMappingOf(context),
                            .choice = context.serverChoice.get()},
      interfaceCanvasOf(context));
}

/** @returns The leaderboard, opened on the normal difficulty and on as many
 * pilots as the waiting room lists, one on the title. */
std::unique_ptr<IScreen> buildLeaderboardScreen(const ScreenContext& context) {
  const std::size_t listed =
      context.services.waitingRoom.get().lobbyPlayers().size();
  return std::make_unique<LeaderboardScreen>(
      LeaderboardScreenParts{
          .navigator = context.navigator.get(),
          .sounds = soundsOf(context),
          .tabs = {LeaderboardTab{
                       .label = SERVER_TAB_LABEL,
                       .source = &context.services.leaderboard.get()},
                   LeaderboardTab{.label = GLOBAL_TAB_LABEL}},
          .choice = {.difficulty = DEFAULT_DIFFICULTY_CHOICE,
                     .pilots = std::max<std::size_t>(listed, 1) - 1},
          .playfieldPointAt = playfieldMappingOf(context),
          .keyName = localizedKeyName,
          .motion = motionPreference()},
      interfaceCanvasOf(context));
}

}  // namespace

std::string ClientMenusModule::name() const {
  return std::string{MENUS_MODULE_NAME};
}

std::vector<std::string> ClientMenusModule::dependencies() const {
  return {std::string{SCREEN_REGISTRY_MODULE_NAME}};
}

void ClientMenusModule::install(engine::InstallContext& context) {
  auto& registry = context.require<ScreenRegistry>();
  registry.add(INTRO_SCREEN, buildIntroScreen);
  registry.add(TITLE_SCREEN, [](const ScreenContext& screens) {
    return buildTitleScreen(screens, TITLE_SCREEN, SquadronStart::FlyingIn);
  });
  registry.add(TITLE_AFTER_INTRO_SCREEN, [](const ScreenContext& screens) {
    return buildTitleScreen(screens, TITLE_AFTER_INTRO_SCREEN,
                            SquadronStart::InPlace);
  });
  registry.add(OPTIONS_SCREEN, [](const ScreenContext& screens) {
    return buildOptionsScreen(screens, OptionsPlace::OverTitle);
  });
  registry.add(GAME_OPTIONS_SCREEN, [](const ScreenContext& screens) {
    return buildOptionsScreen(screens, OptionsPlace::OverGame);
  });
  registry.add(QUIT_CONFIRMATION_SCREEN, buildQuitConfirmationScreen);
  registry.add(CONNECTION_SCREEN, buildConnectionScreen);
  registry.add(LEADERBOARD_SCREEN, buildLeaderboardScreen);
  registry.add(CONNECTION_LOST_SCREEN, buildConnectionLostScreen);
}

void ClientMenusModule::start() {}

void ClientMenusModule::stop() {}

}  // namespace rtype::client
