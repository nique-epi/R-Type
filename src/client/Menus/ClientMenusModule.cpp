#include "ClientMenusModule.hpp"
#include <chrono>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <vector>
#include "BrowserOpening.hpp"
#include "ConnectionGoal.hpp"
#include "ConnectionScreen.hpp"
#include "ConnectionScreenParts.hpp"
#include "ConnectionStart.hpp"
#include "DialogMenus.hpp"
#include "GameListSnapshot.hpp"
#include "HeadphonesScreen.hpp"
#include "HeadphonesScreenParts.hpp"
#include "IScreen.hpp"
#include "IUpdateNotice.hpp"
#include "InstallContext.hpp"
#include "IntroScreen.hpp"
#include "IntroScreenParts.hpp"
#include "LeaderboardScreen.hpp"
#include "LeaderboardScreenConstants.hpp"
#include "LeaderboardScreenParts.hpp"
#include "LeaderboardTab.hpp"
#include "LocalizedKeyName.hpp"
#include "MasterLeaderboard.hpp"
#include "MenuScreenIdConstants.hpp"
#include "ModuleConstants.hpp"
#include "NewGameDialog.hpp"
#include "NewGameMenuParts.hpp"
#include "OpeningScreen.hpp"
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
#include "SignInScreen.hpp"
#include "SignInScreenIdConstants.hpp"
#include "SignInScreenParts.hpp"
#include "SquadronStart.hpp"
#include "TitleOpening.hpp"
#include "TitleScreen.hpp"
#include "TitleScreenParts.hpp"

namespace rtype::client {

namespace {

/** @returns HEADPHONES RECOMMENDED, which gives way to the intro, or to the
 * title when the system asks for less motion. */
std::unique_ptr<IScreen> buildHeadphonesScreen(const ScreenContext& context) {
  return std::make_unique<HeadphonesScreen>(
      HeadphonesScreenParts{.navigator = context.navigator.get(),
                            .next = screenAfterTheHeadphones()},
      interfaceCanvasOf(context));
}

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
 * asks for less motion, and starts as @p opening says otherwise; it asks the
 * notice of @p opening, when there is one. */
std::unique_ptr<IScreen> buildTitleScreen(const ScreenContext& context,
                                          TitleOpening opening) {
  context.services.audio.get().playTheme();
  std::unique_ptr<IScreen> loading =
      loadingScreenUnlessLoaded(context, titleAssets(), opening.screen);
  if (loading != nullptr) {
    return loading;
  }
  return std::make_unique<TitleScreen>(
      TitleScreenParts{.navigator = context.navigator.get(),
                       .sounds = soundsOf(context),
                       .settings = context.services.settings.get(),
                       .wallet = context.services.wallet.get(),
                       .account = context.services.account.get(),
                       .deviceLinks = context.services.deviceLinks.get(),
                       .signInReturn = context.services.signInReturn.get(),
                       .playfieldPointAt = playfieldMappingOf(context),
                       .motion = motionPreference(),
                       .seed = std::random_device{}(),
                       .squadronStart = opening.squadronStart,
                       .playStart = context.services.playStart,
                       .notice = opening.notice},
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

std::unique_ptr<IScreen> buildLeaveGameConfirmationScreen(
    const ScreenContext& context) {
  return dialogScreenOf(context, leaveGameConfirmationMenu(
                                     context.navigator.get(), soundsOf(context),
                                     context.services.connection.get()));
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
                            .account = context.services.account.get(),
                            .tickets = context.services.tickets.get(),
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
                           .audio = context.services.audio.get(),
                           .servers = context.services.serverCatalog.get(),
                           .games = context.services.gameCatalog.get(),
                           .choice = context.serverChoice.get(),
                           .connection = context.services.connection.get(),
                           .wallet = context.services.wallet.get(),
                           .playfieldPointAt = playfieldMappingOf(context),
                           .motion = motionPreference()},
      interfaceCanvasOf(context));
}

/** @returns The new game dialog, offering the difficulties of the server
 * whose games the map listed last. */
std::unique_ptr<IScreen> buildNewGameScreen(const ScreenContext& context) {
  const GameListSnapshot& games = context.services.gameCatalog.get().snapshot();
  return dialogScreenOf(context, newGameMenu(NewGameMenuParts{
                                     .navigator = context.navigator.get(),
                                     .sounds = soundsOf(context),
                                     .choice = context.serverChoice.get(),
                                     .server = games.server,
                                     .difficulties = games.difficulties}));
}

/** @returns The leaderboard, opened on the filter the waiting room calls for;
 * its GLOBAL tab reads the master, with the row of the nickname of the choice.
 */
std::unique_ptr<IScreen> buildLeaderboardScreen(const ScreenContext& context) {
  MasterLeaderboard& global = context.services.globalLeaderboard.get();
  global.namePlayer(context.serverChoice.get().nickname);
  return std::make_unique<LeaderboardScreen>(
      LeaderboardScreenParts{
          .navigator = context.navigator.get(),
          .sounds = soundsOf(context),
          .connection = context.services.connection.get(),
          .tabs = {LeaderboardTab{
                       .label = SERVER_TAB_LABEL,
                       .source = &context.services.leaderboard.get()},
                   LeaderboardTab{.label = GLOBAL_TAB_LABEL,
                                  .source = &global}},
          .choice = OPENING_LEADERBOARD_CHOICE,
          .playfieldPointAt = playfieldMappingOf(context),
          .keyName = localizedKeyName,
          .motion = motionPreference()},
      interfaceCanvasOf(context));
}

/** @returns The dialog that signs the game in through the web site, opening
 * its pages in the browser of the player, on the clock of the system. */
std::unique_ptr<IScreen> buildSignInScreen(const ScreenContext& context) {
  return std::make_unique<SignInScreen>(
      SignInScreenParts{.navigator = context.navigator.get(),
                        .deviceLinks = context.services.deviceLinks.get(),
                        .account = context.services.account.get(),
                        .signInReturn = context.services.signInReturn.get(),
                        .sounds = soundsOf(context),
                        .playfieldPointAt = playfieldMappingOf(context),
                        .openPage = openInBrowser,
                        .now =
                            [] {
                              return std::chrono::floor<std::chrono::seconds>(
                                  std::chrono::system_clock::now());
                            }},
      interfaceCanvasOf(context));
}

/** @brief Adds to @p registry the five connection screens: the form, the one
 * for the leaderboard, the one that joins at once, the one that asks only for
 * the nickname to join a server of the map, and the one RETRY opens. */
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
  registry.add(
      CONNECTION_TO_MAP_SERVER_SCREEN, [](const ScreenContext& screens) {
        return buildConnectionScreen(screens, ConnectionGoal::WaitingRoom,
                                     ConnectionStart::NicknameOnConfirm);
      });
  registry.add(CONNECTION_AGAIN_SCREEN, [](const ScreenContext& screens) {
    return buildConnectionScreen(screens, ConnectionGoal::WaitingRoom,
                                 ConnectionStart::LastAttemptOnConfirm);
  });
}

/** @brief Adds to @p registry the two screens of the sign-in: the dialog
 * that says one is needed, which the hangar pushes over itself, and the
 * dialog that signs the game in through the web site. */
void addSignInScreens(ScreenRegistry& registry) {
  registry.add(SIGN_IN_NEEDED_SCREEN, [](const ScreenContext& screens) {
    return signInNeededScreenOf(screens, std::nullopt);
  });
  registry.add(SIGN_IN_SCREEN, buildSignInScreen);
}

}  // namespace

std::string ClientMenusModule::name() const {
  return std::string{MENUS_MODULE_NAME};
}

std::vector<std::string> ClientMenusModule::dependencies() const {
  return {std::string{SCREENS_MODULE_NAME}};
}

std::vector<std::string> ClientMenusModule::optionalDependencies() const {
  return {std::string{UPDATE_MODULE_NAME}};
}

void ClientMenusModule::install(engine::InstallContext& context) {
  auto& registry = context.require<ScreenRegistry>();
  const IUpdateNotice* notice = context.find<IUpdateNotice>();
  registry.add(HEADPHONES_SCREEN, buildHeadphonesScreen);
  registry.add(INTRO_SCREEN, buildIntroScreen);
  registry.add(TITLE_SCREEN, [notice](const ScreenContext& screens) {
    return buildTitleScreen(
        screens, TitleOpening{.screen = TITLE_SCREEN,
                              .squadronStart = SquadronStart::FlyingIn,
                              .notice = notice});
  });
  registry.add(
      TITLE_AFTER_INTRO_SCREEN, [notice](const ScreenContext& screens) {
        return buildTitleScreen(
            screens, TitleOpening{.screen = TITLE_AFTER_INTRO_SCREEN,
                                  .squadronStart = SquadronStart::InPlace,
                                  .notice = notice});
      });
  registry.add(OPTIONS_SCREEN, [](const ScreenContext& screens) {
    return buildOptionsScreen(screens, OptionsPlace::OverTitle);
  });
  registry.add(GAME_OPTIONS_SCREEN, [](const ScreenContext& screens) {
    return buildOptionsScreen(screens, OptionsPlace::OverGame);
  });
  registry.add(QUIT_CONFIRMATION_SCREEN, buildQuitConfirmationScreen);
  registry.add(LEAVE_GAME_CONFIRMATION_SCREEN,
               buildLeaveGameConfirmationScreen);
  addConnectionScreens(registry);
  registry.add(SERVER_MAP_SCREEN, buildServerMapScreen);
  registry.add(NEW_GAME_SCREEN, buildNewGameScreen);
  registry.add(LEADERBOARD_SCREEN, buildLeaderboardScreen);
  registry.add(CONNECTION_LOST_SCREEN, buildConnectionLostScreen);
  addSignInScreens(registry);
}

void ClientMenusModule::start() {}

void ClientMenusModule::stop() {}

}  // namespace rtype::client
