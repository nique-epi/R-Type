#include "KeyBindings.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <optional>
#include "BoundAction.hpp"
#include "PlayerAction.hpp"

namespace rtype::client {

sf::Keyboard::Scancode KeyBindings::keyOf(BoundAction action) const {
  return keys_[static_cast<std::size_t>(action)];
}

void KeyBindings::bind(BoundAction action, sf::Keyboard::Scancode key) {
  keys_[static_cast<std::size_t>(action)] = key;
}

std::optional<BoundAction> KeyBindings::boundActionFor(
    sf::Keyboard::Scancode key) const {
  for (const BoundAction action : BOUND_ACTIONS) {
    if (keyOf(action) == key) {
      return action;
    }
  }
  return std::nullopt;
}

std::optional<PlayerAction> KeyBindings::playerActionFor(
    sf::Keyboard::Scancode key) const {
  const std::optional<BoundAction> action = boundActionFor(key);
  return action.has_value() ? playerActionOf(*action) : std::nullopt;
}

std::optional<BoundAction> KeyBindings::otherActionWithKeyOf(
    BoundAction action) const {
  for (const BoundAction other : BOUND_ACTIONS) {
    if (other != action && keyOf(other) == keyOf(action)) {
      return other;
    }
  }
  return std::nullopt;
}

std::optional<PlayerAction> KeyBindings::playerActionOf(BoundAction action) {
  switch (action) {
    case BoundAction::MoveUp:
      return PlayerAction::Up;
    case BoundAction::MoveDown:
      return PlayerAction::Down;
    case BoundAction::MoveLeft:
      return PlayerAction::Left;
    case BoundAction::MoveRight:
      return PlayerAction::Right;
    case BoundAction::Shoot:
      return PlayerAction::Shoot;
    case BoundAction::Ready:
      break;
  }
  return std::nullopt;
}

}  // namespace rtype::client
