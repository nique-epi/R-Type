#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

namespace rtype::client::testing {

/** @brief The color of a bulb out in these tests. */
constexpr sf::Color UNLIT_BULB(134, 56, 52);

/** @brief Three bulbs one rank apart, and a bulb whose rank is below zero:
 * -1 shares the phase of rank 2. */
constexpr sf::Vector2i FIRST_BULB(10, 4);
constexpr sf::Vector2i SECOND_BULB(11, 4);
constexpr sf::Vector2i THIRD_BULB(12, 4);
constexpr sf::Vector2i BULB_BELOW_ZERO(13, 4);
constexpr int FIRST_RANK = 0;
constexpr int SECOND_RANK = 1;
constexpr int THIRD_RANK = 2;
constexpr int RANK_BELOW_ZERO = -1;

/** @brief Steps of the chase: the first, the next, and a whole turn of the
 * three phases. */
constexpr int FIRST_STEP = 0;
constexpr int NEXT_STEP = 1;
constexpr int A_WHOLE_TURN = 3;

}  // namespace rtype::client::testing
