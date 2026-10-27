#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client::testing {

/** @brief Points of the playfield and the pixel under each: a pixel covers
 * 4 × 4 units of the playfield. */
constexpr sf::Vector2f LAST_POINT_OF_THE_FIRST_PIXEL(3.9F, 3.9F);
constexpr sf::Vector2i FIRST_PIXEL(0, 0);
constexpr sf::Vector2f POINT_OF_PIXEL_ONE_TWO(4.0F, 8.0F);
constexpr sf::Vector2i PIXEL_ONE_TWO(1, 2);
constexpr sf::Vector2f POINT_LEFT_OF_THE_PICTURE(-0.1F, -0.1F);
constexpr sf::Vector2i PIXEL_LEFT_OF_THE_PICTURE(-1, -1);

}  // namespace rtype::client::testing
