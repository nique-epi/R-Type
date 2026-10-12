#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include "PlayerAction.hpp"

namespace rtype::client {

/**
 * @brief Which key triggers which action.
 *
 * The game only ever sees actions: this is the one place that knows keys.
 * Today the table is fixed (arrows to move, space to shoot); remapping will
 * replace it without touching the code that reads actions.
 */
class KeyBindings {
 public:
  /**
   * @returns The action a key triggers, or nothing when the key is not bound.
   */
  [[nodiscard]] static std::optional<PlayerAction> actionFor(
      sf::Keyboard::Key key);
};

}  // namespace rtype::client
