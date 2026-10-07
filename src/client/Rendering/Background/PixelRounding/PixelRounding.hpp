#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/**
 * @returns The pixel a position is painted on: each coordinate rounded to
 * the nearest integer, halves away from zero.
 */
[[nodiscard]] sf::Vector2i nearestPixel(sf::Vector2f position);

}  // namespace rtype::client
