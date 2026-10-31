#include "ScreenCatalog.hpp"
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <random>
#include <utility>
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "ConnectionScreen.hpp"
#include "ConnectionScreenParts.hpp"
#include "DialogMenus.hpp"
#include "DialogScreen.hpp"
#include "EndScreen.hpp"
#include "EndScreenParts.hpp"
#include "EntityLayer.hpp"
#include "GameScreen.hpp"
#include "GameScreenParts.hpp"
#include "GameWindow.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "ISoundPlayer.hpp"
#include "InterfaceCanvas.hpp"
#include "IntroScreen.hpp"
#include "IntroScreenParts.hpp"
#include "LoadingScreen.hpp"
#include "LobbyPlace.hpp"
#include "LobbyScreen.hpp"
#include "LobbyScreenParts.hpp"
#include "LocalizedKeyName.hpp"
#include "Menu.hpp"
#include "MotionPreference.hpp"
#include "OptionsPlace.hpp"
#include "OptionsScreen.hpp"
#include "OptionsScreenParts.hpp"
#include "PlayerPreferences.hpp"
#include "PlayerSettings.hpp"
#include "PlayfieldMapping.hpp"
#include "ReducedMotion.hpp"
#include "ScreenAssets.hpp"
#include "ScreenCatalogParts.hpp"
#include "ScreenId.hpp"
#include "ServerConnection.hpp"
#include "SquadronStart.hpp"
#include "TitleScreen.hpp"
#include "TitleScreenParts.hpp"

namespace rtype::client {

namespace {

/** @returns Reduced when the system asks for less motion, Full otherwise;
 * read each time a screen that moves is built. */
MotionPreference motionPreference() {
  return systemPrefersReducedMotion() ? MotionPreference::Reduced
                                      : MotionPreference::Full;
}

/** @returns A new picture for a menu, written with the fonts of the asset
 * library. */
std::unique_ptr<IInterfaceCanvas> interfaceCanvas(
    const ScreenCatalogParts& parts) {
  return std::make_unique<InterfaceCanvas>(parts.services.assets.get());
}

/** @returns The player of the short sounds of the audio service. */
ISoundPlayer& sounds(const ScreenCatalogParts& parts) {
  return parts.services.audio.get().sounds();
}

/** @returns What gives the point of the playfield under a pixel of the
 * window. */
PlayfieldMapping playfieldMapping(const ScreenCatalogParts& parts) {
  const GameWindow& window = parts.services.window.get();
  return
      [&window](sf::Vector2i pixel) { return window.playfieldPointAt(pixel); };
}

/** @returns A dialog screen that shows @p menu. */
std::unique_ptr<IScreen> dialogScreen(const ScreenCatalogParts& parts,
                                      Menu menu) {
  return std::make_unique<DialogScreen>(
      std::move(menu), playfieldMapping(parts), interfaceCanvas(parts));
}

}  // namespace

ScreenCatalog::ScreenCatalog(ScreenCatalogParts parts)
    : parts_(std::move(parts)) {}

ScreenId ScreenCatalog::firstScreen() {
  return motionPreference() == MotionPreference::Reduced ? ScreenId::Title
                                                         : ScreenId::Intro;
}

std::unique_ptr<IScreen> ScreenCatalog::create(ScreenId screen) {
  switch (screen) {
    case ScreenId::Intro:
      return introScreen();
    case ScreenId::Title:
    case ScreenId::TitleAfterIntro:
      return titleScreen(screen);
    case ScreenId::Game:
      return gameScreen();
    case ScreenId::Options:
    case ScreenId::GameOptions:
      return optionsScreen(screen);
    case ScreenId::QuitConfirmation:
      return dialogScreen(
          parts_, quitConfirmationMenu(parts_.navigator.get(), sounds(parts_)));
    case ScreenId::Connection:
      return connectionScreen();
    case ScreenId::Lobby:
    case ScreenId::LobbyAfterGame:
      return lobbyScreen(screen);
    case ScreenId::ConnectionLost:
      return dialogScreen(
          parts_,
          connectionLostMenu(parts_.navigator.get(), sounds(parts_),
                             parts_.services.connection.get().status().phase));
    case ScreenId::End:
      return endScreen();
  }
  return nullptr;
}

std::unique_ptr<IScreen> ScreenCatalog::introScreen() const {
  return std::make_unique<IntroScreen>(
      IntroScreenParts{.navigator = parts_.navigator.get(),
                       .settings = parts_.services.settings.get(),
                       .sky = parts_.sky.get(),
                       .sounds = sounds(parts_),
                       .music = parts_.services.audio.get().theme(),
                       .seed = std::random_device{}()},
      interfaceCanvas(parts_));
}

std::unique_ptr<IScreen> ScreenCatalog::connectionScreen() const {
  return std::make_unique<ConnectionScreen>(
      ConnectionScreenParts{.navigator = parts_.navigator.get(),
                            .sounds = sounds(parts_),
                            .connection = parts_.services.connection.get(),
                            .playfieldPointAt = playfieldMapping(parts_),
                            .choice = parts_.serverChoice.get()},
      interfaceCanvas(parts_));
}

std::unique_ptr<IScreen> ScreenCatalog::endScreen() const {
  std::unique_ptr<IScreen> loading =
      loadingScreenUnlessLoaded(gameAssets(), ScreenId::End);
  if (loading != nullptr) {
    return loading;
  }
  const PlayerSettings& settings = parts_.services.settings.get();
  return std::make_unique<EndScreen>(
      EndScreenParts{.navigator = parts_.navigator.get(),
                     .sounds = sounds(parts_),
                     .connection = parts_.services.connection.get(),
                     .result = parts_.gameResult.get(),
                     .previousTaunt = parts_.previousTaunt.get(),
                     .playfieldPointAt = playfieldMapping(parts_),
                     .seed = std::random_device{}(),
                     .motion = motionPreference(),
                     .flashes = flashPreferenceOf(settings),
                     .palette = playerPaletteOf(settings)},
      interfaceCanvas(parts_));
}

std::unique_ptr<IScreen> ScreenCatalog::titleScreen(ScreenId screen) {
  parts_.services.audio.get().playTheme();
  std::unique_ptr<IScreen> loading =
      loadingScreenUnlessLoaded(titleAssets(), screen);
  if (loading != nullptr) {
    return loading;
  }
  return std::make_unique<TitleScreen>(
      TitleScreenParts{.navigator = parts_.navigator.get(),
                       .sounds = sounds(parts_),
                       .settings = parts_.services.settings.get(),
                       .playfieldPointAt = playfieldMapping(parts_),
                       .motion = motionPreference(),
                       .seed = std::random_device{}(),
                       .squadronStart = screen == ScreenId::TitleAfterIntro
                                            ? SquadronStart::InPlace
                                            : SquadronStart::FlyingIn},
      interfaceCanvas(parts_));
}

std::unique_ptr<IScreen> ScreenCatalog::gameScreen() {
  parts_.services.audio.get().playGameMusic();
  std::unique_ptr<IScreen> loading =
      loadingScreenUnlessLoaded(gameAssets(), ScreenId::Game);
  if (loading != nullptr) {
    return loading;
  }
  ServerConnection& connection = parts_.services.connection.get();
  return std::make_unique<GameScreen>(
      GameScreenParts{.navigator = parts_.navigator.get(),
                      .settings = parts_.services.settings.get(),
                      .inputs = connection,
                      .entityMessages = connection,
                      .connection = connection,
                      .sky = parts_.sky.get(),
                      .sounds = sounds(parts_),
                      .result = parts_.gameResult.get()},
      std::make_unique<EntityLayer>(parts_.services.assets.get()),
      interfaceCanvas(parts_));
}

std::unique_ptr<IScreen> ScreenCatalog::optionsScreen(ScreenId screen) {
  return std::make_unique<OptionsScreen>(
      OptionsScreenParts{.navigator = parts_.navigator.get(),
                         .sounds = sounds(parts_),
                         .place = screen == ScreenId::GameOptions
                                      ? OptionsPlace::OverGame
                                      : OptionsPlace::OverTitle,
                         .settings = parts_.services.settings.get(),
                         .storage = parts_.services.storage.get(),
                         .window = parts_.services.window.get(),
                         .keyName = localizedKeyName,
                         .playfieldPointAt = playfieldMapping(parts_)},
      interfaceCanvas(parts_));
}

std::unique_ptr<IScreen> ScreenCatalog::lobbyScreen(ScreenId screen) {
  const bool afterGame = screen == ScreenId::LobbyAfterGame;
  if (afterGame) {
    parts_.services.audio.get().playTheme();
  }
  const PlayerSettings& settings = parts_.services.settings.get();
  return std::make_unique<LobbyScreen>(
      LobbyScreenParts{.navigator = parts_.navigator.get(),
                       .sounds = sounds(parts_),
                       .connection = parts_.services.connection.get(),
                       .playfieldPointAt = playfieldMapping(parts_),
                       .keys = settings.keys,
                       .keyName = localizedKeyName,
                       .palette = playerPaletteOf(settings),
                       .motion = motionPreference(),
                       .place = afterGame ? LobbyPlace::AfterGame
                                          : LobbyPlace::AfterConnection},
      interfaceCanvas(parts_));
}

std::unique_ptr<IScreen> ScreenCatalog::loadingScreenUnlessLoaded(
    const AssetList& assets, ScreenId screen) const {
  AssetLibrary& library = parts_.services.assets.get();
  if (!library.isLoaded(assets)) {
    return std::make_unique<LoadingScreen>(parts_.navigator.get(), library,
                                           assets, screen);
  }
  library.releaseAllExcept(assets);
  return nullptr;
}

}  // namespace rtype::client
