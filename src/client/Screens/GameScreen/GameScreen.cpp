#include "GameScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <cstdint>
#include <memory>
#include <utility>
#include "ActionSet.hpp"
#include "IEntityLayer.hpp"
#include "IScreenNavigator.hpp"
#include "KeyBindings.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "SpriteShowcase.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

GameScreen::GameScreen(IScreenNavigator& navigator,
                       std::unique_ptr<IEntityLayer> entityLayer,
                       const KeyBindings& keys)
    : navigator_(&navigator),
      entityLayer_(std::move(entityLayer)),
      playerCommands_(true, keys) {
  spawnSpriteShowcase(entities_, components_);
  entityLayer_->paint(components_);
}

void GameScreen::handleEvent(const sf::Event& event) {
  const auto* released = event.getIf<sf::Event::KeyReleased>();
  if (released != nullptr && released->code == OPTIONS_KEY) {
    playerCommands_.releaseAll();
    navigator_->push(ScreenId::Options);
  }
  if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
    playerCommands_.keyPressed(keyPressed->scancode);
  } else if (released != nullptr) {
    playerCommands_.keyReleased(released->scancode);
  } else if (event.is<sf::Event::FocusLost>()) {
    playerCommands_.focusLost();
  } else if (event.is<sf::Event::FocusGained>()) {
    playerCommands_.focusGained();
  }
}

void GameScreen::update([[maybe_unused]] engine::Duration elapsed) {
  const std::uint8_t held = playerCommands_.held().bits();
  if (held != loggedActions_) {
    logger_.trace("actions held: ", static_cast<unsigned int>(held));
    loggedActions_ = held;
  }
  entityLayer_->paint(components_);
}

ActionSet GameScreen::playerActions() const { return playerCommands_.held(); }

void GameScreen::draw(sf::RenderTarget& target) const {
  entityLayer_->show(target);
}

bool GameScreen::hidesScreensBelow() const { return true; }

}  // namespace rtype::client
