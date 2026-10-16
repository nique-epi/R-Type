#include "PlayerCommands.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "ActionSet.hpp"
#include "KeyBindings.hpp"
#include "PlayerAction.hpp"

namespace rtype::client {

PlayerCommands::PlayerCommands(bool focused, const KeyBindings& bindings)
    : bindings_(&bindings), focused_(focused) {}

void PlayerCommands::keyPressed(sf::Keyboard::Scancode key) {
  const std::optional<PlayerAction> action = bindings_->playerActionFor(key);
  if (focused_ && action.has_value()) {
    held_.add(*action);
  }
}

void PlayerCommands::keyReleased(sf::Keyboard::Scancode key) {
  const std::optional<PlayerAction> action = bindings_->playerActionFor(key);
  if (focused_ && action.has_value()) {
    held_.remove(*action);
  }
}

void PlayerCommands::focusLost() {
  focused_ = false;
  releaseAll();
}

void PlayerCommands::focusGained() { focused_ = true; }

void PlayerCommands::releaseAll() { held_.clear(); }

ActionSet PlayerCommands::held() const { return held_; }

}  // namespace rtype::client
