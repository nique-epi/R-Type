#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client::testing {

/** @brief A circle of five pixels, its four extremities, and how far a pixel
 * of the midpoint algorithm may lie from the true circle: half a pixel. */
constexpr int FIVE_PIXELS = 5;
constexpr sf::Vector2i RIGHT_END(5, 0);
constexpr sf::Vector2i BOTTOM_END(0, 5);
constexpr sf::Vector2i LEFT_END(-5, 0);
constexpr sf::Vector2i TOP_END(0, -5);
constexpr float HALF_A_PIXEL = 0.5F;

}  // namespace rtype::client::testing
