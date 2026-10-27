#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/**
 * @returns The pixel of the interface picture under a point of the playfield;
 * a point left of or above the picture gives a negative pixel.
 */
[[nodiscard]] sf::Vector2i interfacePixelAt(sf::Vector2f playfieldPoint);

}  // namespace rtype::client
