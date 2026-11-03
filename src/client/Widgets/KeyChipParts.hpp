#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <functional>
#include "BarState.hpp"
#include "Button.hpp"
#include "ISoundPlayer.hpp"

namespace rtype::client {

/**
 * @brief What a KeyChip is built from.
 *
 * A std::reference_wrapper has no default constructor, so this struct is only
 * ever built with designated initializers naming every member without a
 * default: the member-init checks, which report its implicit default
 * constructor, see a constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct KeyChipParts {
  Button::Text text;
  sf::IntRect area;
  sf::Keyboard::Key key{};
  std::function<void()> action;
  /** @brief Plays the click, which must outlive the chip. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief How it looks with no pointer over it: BarState::Selected for the
   * one thing a screen asks of the player. */
  BarState restState = BarState::Idle;
};

}  // namespace rtype::client
