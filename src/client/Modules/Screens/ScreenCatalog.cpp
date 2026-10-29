#include "ScreenCatalog.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <random>
#include <utility>
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "BoundAction.hpp"
#include "ConnectionScreen.hpp"
#include "ConnectionScreenParts.hpp"
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
#include "MotionPreference.hpp"
#include "OptionsPlace.hpp"
#include "OptionsScreen.hpp"
#include "OptionsScreenParts.hpp"
#include "PlayerPreferences.hpp"
#include "PlayerSettings.hpp"
#include "PlayfieldMapping.hpp"
#include "QuitConfirmationScreen.hpp"
#include "QuitConfirmationScreenParts.hpp"
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
      return std::make_unique<IntroScreen>(
          IntroScreenParts{.navigator = parts_.navigator.get(),
                           .settings = parts_.services.settings.get(),
                           .sky = parts_.sky.get(),
                           .sounds = sounds(),
                           .music = parts_.services.audio.get().theme(),
                           .seed = std::random_device{}()},
          interfaceCanvas());
    case ScreenId::Title:
    case ScreenId::TitleAfterIntro:
      return titleScreen(screen);
    case ScreenId::Game:
      return gameScreen();
    case ScreenId::Options:
    case ScreenId::GameOptions:
      return optionsScreen(screen);
    case ScreenId::QuitConfirmation:
      return std::make_unique<QuitConfirmationScreen>(
          QuitConfirmationScreenParts{.navigator = parts_.navigator.get(),
                                      .sounds = sounds(),
                                      .playfieldPointAt = playfieldMapping()},
          interfaceCanvas());
    case ScreenId::Connection:
      return std::make_unique<ConnectionScreen>(
          ConnectionScreenParts{.navigator = parts_.navigator.get(),
                                .sounds = sounds(),
                                .connection = parts_.services.connection.get(),
                                .playfieldPointAt = playfieldMapping(),
                                .server = parts_.services.server,
                                .nickname = parts_.services.nickname},
          interfaceCanvas());
    case ScreenId::Lobby:
    case ScreenId::LobbyAfterGame:
      return lobbyScreen(screen);
    case ScreenId::End: {
      std::unique_ptr<IScreen> loading =
          loadingScreenUnlessLoaded(gameAssets(), screen);
      if (loading != nullptr) {
        return loading;
      }
      const PlayerSettings& settings = parts_.services.settings.get();
      return std::make_unique<EndScreen>(
          EndScreenParts{.navigator = parts_.navigator.get(),
                         .sounds = sounds(),
                         .connection = parts_.services.connection.get(),
                         .result = parts_.gameResult.get(),
                         .previousTaunt = parts_.previousTaunt.get(),
                         .playfieldPointAt = playfieldMapping(),
                         .seed = std::random_device{}(),
                         .motion = motionPreference(),
                         .flashes = flashPreferenceOf(settings),
                         .palette = playerPaletteOf(settings)},
          interfaceCanvas());
    }
  }
  return nullptr;
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
                       .sounds = sounds(),
                       .settings = parts_.services.settings.get(),
                       .playfieldPointAt = playfieldMapping(),
                       .motion = motionPreference(),
                       .seed = std::random_device{}(),
                       .squadronStart = screen == ScreenId::TitleAfterIntro
                                            ? SquadronStart::InPlace
                                            : SquadronStart::FlyingIn},
      interfaceCanvas());
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
                      .sounds = sounds(),
                      .result = parts_.gameResult.get()},
      std::make_unique<EntityLayer>(parts_.services.assets.get()),
      interfaceCanvas());
}

std::unique_ptr<IScreen> ScreenCatalog::optionsScreen(ScreenId screen) {
  return std::make_unique<OptionsScreen>(
      OptionsScreenParts{.navigator = parts_.navigator.get(),
                         .sounds = sounds(),
                         .place = screen == ScreenId::GameOptions
                                      ? OptionsPlace::OverGame
                                      : OptionsPlace::OverTitle,
                         .settings = parts_.services.settings.get(),
                         .storage = parts_.services.storage.get(),
                         .window = parts_.services.window.get(),
                         .keyName = localizedKeyName,
                         .playfieldPointAt = playfieldMapping()},
      interfaceCanvas());
}

std::unique_ptr<IScreen> ScreenCatalog::lobbyScreen(ScreenId screen) {
  const bool afterGame = screen == ScreenId::LobbyAfterGame;
  if (afterGame) {
    parts_.services.audio.get().playTheme();
  }
  const PlayerSettings& settings = parts_.services.settings.get();
  const sf::Keyboard::Scancode readyKey =
      settings.keys.keyOf(BoundAction::Ready);
  return std::make_unique<LobbyScreen>(
      LobbyScreenParts{
          .navigator = parts_.navigator.get(),
          .sounds = sounds(),
          .connection = parts_.services.connection.get(),
          .playfieldPointAt = playfieldMapping(),
          .readyKey = {.key = readyKey, .name = localizedKeyName(readyKey)},
          .palette = playerPaletteOf(settings),
          .motion = motionPreference(),
          .place =
              afterGame ? LobbyPlace::AfterGame : LobbyPlace::AfterConnection},
      interfaceCanvas());
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

std::unique_ptr<IInterfaceCanvas> ScreenCatalog::interfaceCanvas() const {
  return std::make_unique<InterfaceCanvas>(parts_.services.assets.get());
}

ISoundPlayer& ScreenCatalog::sounds() const {
  return parts_.services.audio.get().sounds();
}

PlayfieldMapping ScreenCatalog::playfieldMapping() const {
  const GameWindow& window = parts_.services.window.get();
  return
      [&window](sf::Vector2i pixel) { return window.playfieldPointAt(pixel); };
}

}  // namespace rtype::client
