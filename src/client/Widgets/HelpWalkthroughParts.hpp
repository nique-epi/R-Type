#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <functional>
#include "ISoundPlayer.hpp"
#include "KeyBindings.hpp"
#include "KeyNames.hpp"
#include "MotionPreference.hpp"
#include "PlayerPalette.hpp"

namespace rtype::client {

/**
 * @brief What a help is built from.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member without
 * a default, as LobbyScreenParts is.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct HelpWalkthroughParts {
  /** @brief Plays the click of each step, which must outlive the help. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief The keys the player chose, named once when the help is built. */
  std::reference_wrapper<const KeyBindings> keys;
  /** @brief Gives the name a keycap shows for a key. */
  KeyNaming keyName;
  /** @brief The palette of the ships of its moment of a game: a constant, so
   * it outlives the help. */
  std::reference_wrapper<const PlayerPalette> palette;
  /** @brief Whether the professor, the corners and the ships may move. */
  MotionPreference motion{};
  /** @brief Where its HELP chip lies, in pixels of the interface picture. */
  sf::IntRect chipArea;
};

}  // namespace rtype::client
