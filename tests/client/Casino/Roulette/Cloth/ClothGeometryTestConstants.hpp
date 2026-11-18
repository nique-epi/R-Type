#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include "ClothBand.hpp"
#include "ClothPlace.hpp"

namespace rtype::client::testing {

/** @brief 17 on the picture of the cloth: the sixth street, whose line on
 * the left is at x 172 + 5 x 21 = 277, the middle row, whose line above is at
 * y 100 + 18 = 118; its box, both lines included, and its middle. */
constexpr std::uint8_t NUMBER_SEVENTEEN = 17;
constexpr ClothPlace PLACE_OF_SEVENTEEN{.across = 11, .down = 2};
constexpr sf::IntRect BOX_OF_SEVENTEEN({277, 118}, {22, 19});
constexpr sf::Vector2i MIDDLE_OF_SEVENTEEN(287, 127);

/** @brief Where 17, 18, 20 and 21 meet: the line after 17, x 298, and the
 * line above it, y 118. */
constexpr ClothPlace CORNER_OF_SEVENTEEN{.across = 12, .down = 1};
constexpr sf::Vector2i POINT_OF_THE_CORNER(298, 118);

/** @brief The zero, at the head of the three rows: 18 wide, 54 tall. */
constexpr sf::IntRect BOX_OF_THE_ZERO({154, 100}, {19, 55});

/** @brief The second dozen, under 13 to 24: 84 wide, from x 256, under the
 * last row at y 154. */
constexpr ClothPlace SECOND_DOZEN{.band = ClothBand::Dozens, .across = 1};
constexpr sf::IntRect BOX_OF_THE_SECOND_DOZEN({256, 154}, {85, 16});

}  // namespace rtype::client::testing
