#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <optional>
#include "BoundAction.hpp"
#include "InputConstants.hpp"
#include "PlayerAction.hpp"

namespace rtype::client {

/**
 * @brief Which physical key triggers each bound action.
 *
 * Keys are scancodes: a binding names the place of a key on the keyboard,
 * not the character the layout prints on it, so the keys of a first launch
 * fall under the same fingers on every layout. A key may be bound to several
 * actions: the options show it as a conflict, and the first of them in the
 * order of BOUND_ACTIONS takes the key. The game only ever sees actions; this
 * is the one place that knows keys.
 */
class KeyBindings {
 public:
  /** @brief The keys of a first launch, DEFAULT_KEYS. */
  KeyBindings() = default;

  [[nodiscard]] sf::Keyboard::Scancode keyOf(BoundAction action) const;

  void bind(BoundAction action, sf::Keyboard::Scancode key);

  /** @returns The first action bound to the key, in the order of
   * BOUND_ACTIONS, or nothing. */
  [[nodiscard]] std::optional<BoundAction> boundActionFor(
      sf::Keyboard::Scancode key) const;

  /** @returns The action of a PlayerInput the key triggers, or nothing for a
   * key that neither moves nor shoots. */
  [[nodiscard]] std::optional<PlayerAction> playerActionFor(
      sf::Keyboard::Scancode key) const;

  /** @returns Another action bound to the key of this one, the first in the
   * order of BOUND_ACTIONS, or nothing. */
  [[nodiscard]] std::optional<BoundAction> otherActionWithKeyOf(
      BoundAction action) const;

  bool operator==(const KeyBindings& other) const = default;

 private:
  /** @returns The action of a PlayerInput a bound action stands for, or
   * nothing for Ready. */
  [[nodiscard]] static std::optional<PlayerAction> playerActionOf(
      BoundAction action);

  std::array<sf::Keyboard::Scancode, BOUND_ACTION_COUNT> keys_ = DEFAULT_KEYS;
};

}  // namespace rtype::client
