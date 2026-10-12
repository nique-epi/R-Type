#include "PlayerCommands.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "ActionSet.hpp"
#include "KeyBindings.hpp"
#include "PlayerAction.hpp"

namespace rtype::client {

PlayerCommands::PlayerCommands(bool focused) : focused_(focused) {}

void PlayerCommands::keyPressed(sf::Keyboard::Key key) {
  const std::optional<PlayerAction> action = KeyBindings::actionFor(key);
  if (focused_ && action.has_value()) {
    held_.add(*action);
  }
}

void PlayerCommands::keyReleased(sf::Keyboard::Key key) {
  const std::optional<PlayerAction> action = KeyBindings::actionFor(key);
  if (focused_ && action.has_value()) {
    held_.remove(*action);
  }
}

void PlayerCommands::focusLost() {
  focused_ = false;
  held_.clear();
}

void PlayerCommands::focusGained() { focused_ = true; }

ActionSet PlayerCommands::held() const { return held_; }

}  // namespace rtype::client
