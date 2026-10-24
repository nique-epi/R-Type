#pragma once

#include <SFML/Graphics/Color.hpp>

namespace rtype::client {

/** @brief The two shades of a player color: the main color of its ship, what
 * the interface paints in the color of the player, and its light shade, the
 * nickname of the player written on the sky. */
struct PlayerColors {
  sf::Color ship;
  sf::Color light;
};

}  // namespace rtype::client
