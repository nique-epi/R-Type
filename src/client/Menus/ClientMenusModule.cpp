#include "ClientMenusModule.hpp"
#include <memory>
#include <random>
#include <string>
#include <vector>
#include "ConnectionGoal.hpp"
#include "ConnectionScreen.hpp"
#include "ConnectionScreenParts.hpp"
#include "ConnectionStart.hpp"
#include "DialogMenus.hpp"
#include "IScreen.hpp"
#include "InstallContext.hpp"
#include "IntroScreen.hpp"
#include "IntroScreenParts.hpp"
#include "LeaderboardPresentation.hpp"
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
#include "ServerMapScreen.hpp"
#include "ServerMapScreenParts.hpp"
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
                       .squadronStart = squadronStart,
                       .playStart = context.services.playStart},
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

std::unique_ptr<IScreen> buildConnectionScreen(
    const ScreenContext& context, ConnectionGoal goal,
    ConnectionStart start = ConnectionStart::OnConfirm) {
  return std::make_unique<ConnectionScreen>(
      ConnectionScreenParts{.navigator = context.navigator.get(),
                            .sounds = soundsOf(context),
                            .connection = context.services.connection.get(),
                            .playfieldPointAt = playfieldMappingOf(context),
                            .choice = context.serverChoice.get(),
                            .goal = goal,
                            .start = start},
      interfaceCanvasOf(context));
}

std::unique_ptr<IScreen> buildServerMapScreen(const ScreenContext& context) {
  return std::make_unique<ServerMapScreen>(
      ServerMapScreenParts{.navigator = context.navigator.get(),
                           .sounds = soundsOf(context),
                           .servers = context.services.serverCatalog.get(),
                           .games = context.services.gameCatalog.get(),
                           .choice = context.serverChoice.get(),
                           .playfieldPointAt = playfieldMappingOf(context),
                           .motion = motionPreference()},
      interfaceCanvasOf(context));
}

/** @returns The leaderboard, opened on the filter the waiting room calls for.
 */
std::unique_ptr<IScreen> buildLeaderboardScreen(const ScreenContext& context) {
  return std::make_unique<LeaderboardScreen>(
      LeaderboardScreenParts{
          .navigator = context.navigator.get(),
          .sounds = soundsOf(context),
          .connection = context.services.connection.get(),
          .tabs = {LeaderboardTab{
                       .label = SERVER_TAB_LABEL,
                       .source = &context.services.leaderboard.get()},
                   LeaderboardTab{.label = GLOBAL_TAB_LABEL}},
          .choice = openingChoiceOf(
              context.services.waitingRoom.get().lobbyPlayers().size()),
          .playfieldPointAt = playfieldMappingOf(context),
          .keyName = localizedKeyName,
          .motion = motionPreference()},
      interfaceCanvasOf(context));
}

/** @brief Adds to @p registry the four connection screens: the form, the one
 * for the leaderboard, the one that joins at once and the one RETRY opens. */
void addConnectionScreens(ScreenRegistry& registry) {
  registry.add(CONNECTION_SCREEN, [](const ScreenContext& screens) {
    return buildConnectionScreen(screens, ConnectionGoal::WaitingRoom);
  });
  registry.add(
      CONNECTION_FOR_LEADERBOARD_SCREEN, [](const ScreenContext& screens) {
        return buildConnectionScreen(screens, ConnectionGoal::ScreenBelow);
      });
  registry.add(
      CONNECTION_TO_CHOSEN_SERVER_SCREEN, [](const ScreenContext& screens) {
        return buildConnectionScreen(screens, ConnectionGoal::WaitingRoom,
                                     ConnectionStart::AtOnce);
      });
  registry.add(CONNECTION_AGAIN_SCREEN, [](const ScreenContext& screens) {
    return buildConnectionScreen(screens, ConnectionGoal::WaitingRoom,
                                 ConnectionStart::LastAttemptOnConfirm);
  });
}

}  // namespace

std::string ClientMenusModule::name() const {
  return std::string{MENUS_MODULE_NAME};
}

std::vector<std::string> ClientMenusModule::dependencies() const {
  return {std::string{SCREENS_MODULE_NAME}};
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
  addConnectionScreens(registry);
  registry.add(SERVER_MAP_SCREEN, buildServerMapScreen);
  registry.add(LEADERBOARD_SCREEN, buildLeaderboardScreen);
  registry.add(CONNECTION_LOST_SCREEN, buildConnectionLostScreen);
}

void ClientMenusModule::start() {}

void ClientMenusModule::stop() {}

}  // namespace rtype::client
