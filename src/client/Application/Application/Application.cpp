#include "Application.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <filesystem>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <utility>
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "ConnectionScreen.hpp"
#include "Endpoint.hpp"
#include "EntityLayer.hpp"
#include "GameScreen.hpp"
#include "IInterfaceCanvas.hpp"
#include "IScreen.hpp"
#include "InterfaceCanvas.hpp"
#include "LoadingScreen.hpp"
#include "LobbyScreen.hpp"
#include "LocalizedKeyName.hpp"
#include "MotionPreference.hpp"
#include "NetworkConstants.hpp"
#include "OptionsScreen.hpp"
#include "PlayerSettings.hpp"
#include "PlayfieldMapping.hpp"
#include "QuitConfirmationScreen.hpp"
#include "ReducedMotion.hpp"
#include "ScreenAssets.hpp"
#include "ScreenId.hpp"
#include "ScreenStack.hpp"
#include "SettingsFile.hpp"
#include "TimeConstants.hpp"
#include "TitleScreen.hpp"

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

}  // namespace

Application::Application(AssetLibrary& assets, network::Endpoint server,
                         std::string nickname,
                         std::optional<std::filesystem::path> settingsFile)
    : assets_(&assets),
      settingsFile_(std::move(settingsFile)),
      settings_(settingsFile_.load(firstLaunchSettings())),
      window_(settings_.windowSize),
      background_(std::random_device{}()),
      network_(
          network::Endpoint{.address = std::string{network::ANY_IPV4_ADDRESS},
                            .port = network::ANY_PORT}),
      connection_(network_),
      server_(std::move(server)),
      nickname_(std::move(nickname)),
      screens_([this](ScreenId screen) { return createScreen(screen); }) {
  screens_.replaceAll(ScreenId::Title);
  screens_.applyPendingChanges();
}

void Application::run() {
  previousFrameTime_ = clock_.now();
  while (window_.isOpen() && !screens_.isEmpty()) {
    handleEvents();
    const engine::Duration elapsed = measureFrame();
    connection_.update(elapsed);
    background_.advance(elapsed);
    screens_.update(elapsed);
    screens_.applyPendingChanges();
    render();
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
    case ScreenId::Title:
      return titleScreen();
    case ScreenId::Game:
      return gameScreen();
    case ScreenId::Options:
    case ScreenId::GameOptions:
      return optionsScreen(screen);
    case ScreenId::QuitConfirmation:
      return std::make_unique<QuitConfirmationScreen>(
          screens_, interfaceCanvas(), playfieldMapping());
    case ScreenId::Connection:
      return std::make_unique<ConnectionScreen>(
          screens_, connection_, interfaceCanvas(), playfieldMapping(), server_,
          nickname_);
    case ScreenId::Lobby:
      return std::make_unique<LobbyScreen>(
          screens_, connection_, interfaceCanvas(), playfieldMapping(),
          settings_.keys, localizedKeyName, motionPreference());
  }
  return nullptr;
}

std::unique_ptr<IScreen> Application::titleScreen() {
  std::unique_ptr<IScreen> loading =
      loadingScreenUnlessLoaded(titleAssets(), ScreenId::Title);
  if (loading != nullptr) {
    return loading;
  }
  return std::make_unique<TitleScreen>(screens_, interfaceCanvas(),
                                       playfieldMapping(), motionPreference(),
                                       std::random_device{}());
}

std::unique_ptr<IScreen> Application::gameScreen() {
  std::unique_ptr<IScreen> loading =
      loadingScreenUnlessLoaded(gameAssets(), ScreenId::Game);
  if (loading != nullptr) {
    return loading;
  }
  return std::make_unique<GameScreen>(
      screens_, std::make_unique<EntityLayer>(*assets_), settings_);
}

std::unique_ptr<IScreen> Application::optionsScreen(ScreenId screen) {
  const OptionsScreen::Place place = screen == ScreenId::GameOptions
                                         ? OptionsScreen::Place::OverGame
                                         : OptionsScreen::Place::OverTitle;
  return std::make_unique<OptionsScreen>(
      screens_, place, settings_, settingsFile_, window_, localizedKeyName,
      interfaceCanvas(), playfieldMapping());
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
