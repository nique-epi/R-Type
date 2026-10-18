#include "GameScreen.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <utility>
#include "ActionSet.hpp"
#include "IEntityLayer.hpp"
#include "IPlayerInputLink.hpp"
#include "IScreenNavigator.hpp"
#include "KeyBindings.hpp"
#include "PlayerInputPacer.hpp"
#include "ScreenConstants.hpp"
#include "ScreenId.hpp"
#include "SpriteShowcase.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

GameScreen::GameScreen(IScreenNavigator& navigator,
                       std::unique_ptr<IEntityLayer> entityLayer,
                       const KeyBindings& keys, IPlayerInputLink& inputs)
    : navigator_(&navigator),
      entityLayer_(std::move(entityLayer)),
      playerCommands_(true, keys),
      inputs_(&inputs) {
  spawnSpriteShowcase(entities_, components_);
  entityLayer_->paint(components_);
}

void GameScreen::handleEvent(const sf::Event& event) {
  const auto* released = event.getIf<sf::Event::KeyReleased>();
  if (released != nullptr && released->code == OPTIONS_KEY) {
    playerCommands_.releaseAll();
    navigator_->push(ScreenId::GameOptions);
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

void GameScreen::update(engine::Duration elapsed) {
  const ActionSet held = playerCommands_.held();
  if (inputPacer_.advance(elapsed)) {
    inputs_->sendPlayerInput(held);
  }
  if (held.bits() != loggedActions_) {
    logger_.trace("actions held: ", static_cast<unsigned int>(held.bits()));
    loggedActions_ = held.bits();
  }
  entityLayer_->paint(components_);
}

ActionSet GameScreen::playerActions() const { return playerCommands_.held(); }

void GameScreen::draw(sf::RenderTarget& target) const {
  entityLayer_->show(target);
}

bool GameScreen::hidesScreensBelow() const { return true; }

}  // namespace rtype::client
