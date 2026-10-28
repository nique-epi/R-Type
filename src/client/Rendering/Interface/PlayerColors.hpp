#pragma once

#include <SFML/Graphics/Color.hpp>

namespace rtype::client {

/** @brief The three shades of a player color: the dark one, the edges of its
 * ship and the end of its debris; the main one, its ship and what the
 * interface paints in the color of the player; the light one, the nickname of
 * the player written on the sky. */
struct PlayerColors {
  sf::Color dark;
  sf::Color ship;
  sf::Color light;

  bool operator==(const PlayerColors& other) const = default;
};

}  // namespace rtype::client
