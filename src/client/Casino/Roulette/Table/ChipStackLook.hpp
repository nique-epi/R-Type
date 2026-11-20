#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @brief How a stack of chips is drawn on its spot: moved by `offset`, its
 * chips multiplied by `tint`, whose alpha lets the cloth show through. The
 * stacks of the player sit on their spot, untinted. */
struct ChipStackLook {
  sf::Vector2i offset;
  sf::Color tint = sf::Color::White;
};

}  // namespace rtype::client
