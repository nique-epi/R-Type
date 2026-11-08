#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>

namespace rtype::client::testing {

/** @brief A color the shapes are drawn in, and the pixels of a segment from
 * ORIGIN to FOUR_RIGHT: five, its two ends included. */
constexpr sf::Color SHAPE_COLOR(1, 2, 3);
constexpr sf::Vector2i ORIGIN(0, 0);
constexpr sf::Vector2i FOUR_RIGHT(4, 0);
constexpr std::size_t PIXELS_FROM_ORIGIN_TO_FOUR_RIGHT = 5;

/** @brief Three pixels thick, that segment covers columns -1 to 5 and rows -1
 * to 1: 7 x 3 distinct pixels. */
constexpr int THREE_PIXELS_THICK = 3;
constexpr std::size_t THICK_SEGMENT_PIXELS = 21;

/** @brief Discs: radius 2 has rows of 3, 5, 5, 5 and 3 pixels. */
constexpr int RADIUS_TWO = 2;
constexpr std::size_t RADIUS_TWO_PIXELS = 21;
constexpr int NO_RADIUS = -1;

/** @brief A patch of four by four pixels, and the threshold of the dither at
 * the origin: the lowest level, half a level up, out of sixteen. */
constexpr sf::IntRect FOUR_BY_FOUR{{0, 0}, {4, 4}};
constexpr std::size_t FOUR_BY_FOUR_PIXELS = 16;
constexpr double LOWEST_THRESHOLD = 0.5 / 16.0;

}  // namespace rtype::client::testing
