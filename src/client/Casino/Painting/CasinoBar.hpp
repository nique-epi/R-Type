#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "BarState.hpp"
#include "Translation.hpp"

namespace rtype::client {

/** @brief A bar of a screen of the casino, but what it does: its label, the
 * name on its keycap, where it lies, its key, and how it looks at rest. */
struct CasinoBar {
  Translation label;
  const char* keyName{};
  sf::IntRect area;
  sf::Keyboard::Key key{};
  /** @brief BarState::Selected for the one thing a screen asks of the
   * player. */
  BarState restState = BarState::Idle;
};

}  // namespace rtype::client
