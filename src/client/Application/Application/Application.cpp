#include "Application.hpp"
#include <SFML/Window/Event.hpp>
#include <memory>
#include <optional>
#include <random>
#include "AssetLibrary.hpp"
#include "GameScreen.hpp"
#include "LoadingScreen.hpp"
#include "OptionsScreen.hpp"
#include "ScreenAssets.hpp"
#include "ScreenConstants.hpp"
#include "ScreenStack.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

Application::Application(AssetLibrary& assets)
    : assets_(&assets), background_(std::random_device{}()) {
  screens_.replaceAll(gameLoadingScreen());
  screens_.applyPendingChanges();
}

void Application::run() {
  previousFrameTime_ = clock_.now();
  while (window_.isOpen() && !screens_.isEmpty()) {
    handleEvents();
    advanceBackground();
    screens_.update();
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

void Application::advanceBackground() {
  const engine::Duration now = clock_.now();
  background_.advance(now - previousFrameTime_);
  previousFrameTime_ = now;
}

void Application::render() {
  window_.clear();
  background_.draw(window_.renderTarget());
  screens_.draw(window_.renderTarget());
  window_.display();
}

ScreenFactory Application::gameLoadingScreen() {
  return [this] {
    return std::make_unique<LoadingScreen>(screens_, *assets_, gameAssets(),
                                           gameScreen());
  };
}

ScreenFactory Application::gameScreen() {
  return [this] {
    return std::make_unique<GameScreen>(screens_, optionsScreen());
  };
}

ScreenFactory Application::optionsScreen() {
  return [this] {
    return std::make_unique<OptionsScreen>(screens_, window_,
                                           assets_->font(INTERFACE_FONT_ID));
  };
}

}  // namespace rtype::client
