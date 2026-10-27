#include "Application.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <filesystem>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <thread>
#include <utility>
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "AudioConstants.hpp"
#include "BoundAction.hpp"
#include "ConnectionConstants.hpp"
#include "ConnectionScreen.hpp"
#include "ConnectionScreenParts.hpp"
#include "EndScreen.hpp"
#include "EndScreenDemo.hpp"
#include "EndScreenParts.hpp"
#include "Endpoint.hpp"
#include "EntityLayer.hpp"
#include "GameResult.hpp"
#include "GameScreen.hpp"
#include "GameScreenParts.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "InterfaceCanvas.hpp"
#include "IntroScreen.hpp"
#include "IntroScreenParts.hpp"
#include "LaunchOptions.hpp"
#include "LoadingScreen.hpp"
#include "LobbyPlace.hpp"
#include "LobbyScreen.hpp"
#include "LobbyScreenParts.hpp"
#include "LocalizedKeyName.hpp"
#include "MotionPreference.hpp"
#include "NetworkConstants.hpp"
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
#include "ScreenId.hpp"
#include "ScreenStack.hpp"
#include "SettingsFile.hpp"
#include "SoundtrackParts.hpp"
#include "SquadronStart.hpp"
#include "TimeConstants.hpp"
#include "TitleScreen.hpp"
#include "TitleScreenParts.hpp"

namespace rtype::client {

namespace {

/** @returns The settings of a first launch, flashes reduced when the system
 * asks for less motion. */
PlayerSettings firstLaunchSettings() {
  PlayerSettings settings;
  settings.reduceFlashes = systemPrefersReducedMotion();
  return settings;
}

/** @returns Reduced when the system asks for less motion, Full otherwise;
 * read each time a screen that moves is built. */
MotionPreference motionPreference() {
  return systemPrefersReducedMotion() ? MotionPreference::Reduced
                                      : MotionPreference::Full;
}

/** @returns The screen the client opens on: the end screen of a sample game
 * for --end-screen-demo; else the intro, or the title when the system asks
 * for less motion. */
ScreenId firstScreen(bool endScreenDemo) {
  if (endScreenDemo) {
    return ScreenId::End;
  }
  return motionPreference() == MotionPreference::Reduced ? ScreenId::Title
                                                         : ScreenId::Intro;
}

}  // namespace

Application::Application(AssetLibrary& assets, const LaunchOptions& launch,
                         std::optional<std::filesystem::path> settingsFile)
    : assets_(&assets),
      settingsFile_(std::move(settingsFile)),
      settings_(settingsFile_.load(firstLaunchSettings())),
      window_(settings_.windowSize),
      background_(std::random_device{}()),
      sounds_(assets, settings_),
      theme_(assets, settings_, THEME_TRACK),
      gameMusic_(assets, settings_, GAME_TRACK),
      soundtrack_(SoundtrackParts{.theme = theme_, .gameMusic = gameMusic_}),
      network_(
          network::Endpoint{.address = std::string{network::ANY_IPV4_ADDRESS},
                            .port = network::ANY_PORT}),
      connection_(network_),
      server_(launch.serverEndpoint()),
      nickname_(launch.nickname()),
      gameResult_(launch.endScreenDemo() ? demoGameResult() : GameResult{}),
      screens_([this](ScreenId screen) { return createScreen(screen); }) {
  screens_.replaceAll(firstScreen(launch.endScreenDemo()));
  screens_.applyPendingChanges();
}

void Application::run() {
  previousFrameTime_ = clock_.now();
  while (window_.isOpen() && !screens_.isEmpty()) {
    handleEvents();
    const engine::Duration elapsed = measureFrame();
    connection_.update(elapsed);
    theme_.update();
    gameMusic_.update();
    soundtrack_.advance(elapsed);
    background_.advance(elapsed);
    screens_.update(elapsed);
    screens_.applyPendingChanges();
    render();
  }
  leaveServer();
}

void Application::leaveServer() {
  connection_.cancel();
  while (connection_.isLeaving()) {
    std::this_thread::sleep_for(DISCONNECTION_COPY_INTERVAL);
    connection_.update(measureFrame());
  }
}

void Application::handleEvents() {
  while (const std::optional<sf::Event> event = window_.pollScreenEvent()) {
    screens_.handleEvent(*event);
    screens_.applyPendingChanges();
  }
}

engine::Duration Application::measureFrame() {
  const engine::Duration now = clock_.now();
  const engine::Duration elapsed = now - previousFrameTime_;
  previousFrameTime_ = now;
  return elapsed;
}

void Application::render() {
  window_.clear();
  background_.draw(window_.renderTarget());
  screens_.draw(window_.renderTarget());
  window_.display();
}

std::unique_ptr<IScreen> Application::createScreen(ScreenId screen) {
  switch (screen) {
    case ScreenId::Intro:
      return std::make_unique<IntroScreen>(
          IntroScreenParts{.navigator = screens_,
                           .settings = settings_,
                           .sky = background_,
                           .sounds = sounds_,
                           .music = theme_,
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
          QuitConfirmationScreenParts{.navigator = screens_,
                                      .sounds = sounds_,
                                      .playfieldPointAt = playfieldMapping()},
          interfaceCanvas());
    case ScreenId::Connection:
      return std::make_unique<ConnectionScreen>(
          ConnectionScreenParts{.navigator = screens_,
                                .sounds = sounds_,
                                .connection = connection_,
                                .playfieldPointAt = playfieldMapping(),
                                .server = server_,
                                .nickname = nickname_},
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
      return std::make_unique<EndScreen>(
          EndScreenParts{.navigator = screens_,
                         .sounds = sounds_,
                         .connection = connection_,
                         .result = gameResult_,
                         .previousTaunt = previousTaunt_,
                         .playfieldPointAt = playfieldMapping(),
                         .seed = std::random_device{}(),
                         .motion = motionPreference(),
                         .flashes = flashPreferenceOf(settings_)},
          interfaceCanvas());
    }
  }
  return nullptr;
}

std::unique_ptr<IScreen> Application::titleScreen(ScreenId screen) {
  soundtrack_.playTheme();
  std::unique_ptr<IScreen> loading =
      loadingScreenUnlessLoaded(titleAssets(), screen);
  if (loading != nullptr) {
    return loading;
  }
  return std::make_unique<TitleScreen>(
      TitleScreenParts{.navigator = screens_,
                       .sounds = sounds_,
                       .playfieldPointAt = playfieldMapping(),
                       .motion = motionPreference(),
                       .seed = std::random_device{}(),
                       .squadronStart = screen == ScreenId::TitleAfterIntro
                                            ? SquadronStart::InPlace
                                            : SquadronStart::FlyingIn},
      interfaceCanvas());
}

std::unique_ptr<IScreen> Application::gameScreen() {
  soundtrack_.playGameMusic();
  std::unique_ptr<IScreen> loading =
      loadingScreenUnlessLoaded(gameAssets(), ScreenId::Game);
  if (loading != nullptr) {
    return loading;
  }
  return std::make_unique<GameScreen>(
      GameScreenParts{.navigator = screens_,
                      .settings = settings_,
                      .inputs = connection_,
                      .entityMessages = connection_,
                      .connection = connection_,
                      .sky = background_,
                      .sounds = sounds_,
                      .result = gameResult_},
      std::make_unique<EntityLayer>(*assets_), interfaceCanvas());
}

std::unique_ptr<IScreen> Application::optionsScreen(ScreenId screen) {
  return std::make_unique<OptionsScreen>(
      OptionsScreenParts{.navigator = screens_,
                         .sounds = sounds_,
                         .place = screen == ScreenId::GameOptions
                                      ? OptionsPlace::OverGame
                                      : OptionsPlace::OverTitle,
                         .settings = settings_,
                         .storage = settingsFile_,
                         .window = window_,
                         .keyName = localizedKeyName,
                         .playfieldPointAt = playfieldMapping()},
      interfaceCanvas());
}

std::unique_ptr<IScreen> Application::lobbyScreen(ScreenId screen) {
  const bool afterGame = screen == ScreenId::LobbyAfterGame;
  if (afterGame) {
    soundtrack_.playTheme();
  }
  const sf::Keyboard::Scancode readyKey =
      settings_.keys.keyOf(BoundAction::Ready);
  return std::make_unique<LobbyScreen>(
      LobbyScreenParts{
          .navigator = screens_,
          .sounds = sounds_,
          .connection = connection_,
          .playfieldPointAt = playfieldMapping(),
          .readyKey = {.key = readyKey, .name = localizedKeyName(readyKey)},
          .motion = motionPreference(),
          .place =
              afterGame ? LobbyPlace::AfterGame : LobbyPlace::AfterConnection},
      interfaceCanvas());
}

std::unique_ptr<IScreen> Application::loadingScreenUnlessLoaded(
    const AssetList& assets, ScreenId screen) {
  if (!assets_->isLoaded(assets)) {
    return std::make_unique<LoadingScreen>(screens_, *assets_, assets, screen);
  }
  assets_->releaseAllExcept(assets);
  return nullptr;
}

std::unique_ptr<IInterfaceCanvas> Application::interfaceCanvas() const {
  return std::make_unique<InterfaceCanvas>(*assets_);
}

PlayfieldMapping Application::playfieldMapping() const {
  return [this](sf::Vector2i pixel) { return window_.playfieldPointAt(pixel); };
}

}  // namespace rtype::client
