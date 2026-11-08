#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/**
 * @returns The pixel a position is painted on: each coordinate rounded to
 * the nearest integer, halves away from zero.
 */
[[nodiscard]] sf::Vector2i nearestPixel(sf::Vector2f position);

/** @returns The pixel @p share of the way from @p start to @p end, rounded
 * as nearestPixel() does. */
[[nodiscard]] sf::Vector2i pixelAlong(sf::Vector2i start, sf::Vector2i end,
                                      float share);

}  // namespace rtype::client
