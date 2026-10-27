#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief The second bay of the waiting room the team chose, and where its
 * parts lie: the nickname on its middle column at row 120, the ship 19 in and
 * 30 down, the inside of its cell, YOU at the end of the number row. */
constexpr sf::Vector2i BAY_TOP_LEFT(134, 50);
constexpr sf::IntRect BAY_AREA({134, 50}, {102, 112});
constexpr sf::Vector2i NICKNAME_ANCHOR(185, 120);
constexpr sf::Vector2i SHIP_TOP_LEFT(153, 80);
constexpr sf::IntRect CELL_INSIDE({143, 137}, {84, 14});
constexpr sf::Vector2i LOCAL_TAG_ANCHOR(228, 58);
/** @brief The two frames of the third ship color of the entity sheet, and the
 * enlargement of a ship in a bay. */
constexpr sf::IntRect THIRD_COLOR_FIRST_FRAME({0, 36}, {32, 16});
constexpr sf::IntRect THIRD_COLOR_SECOND_FRAME({34, 36}, {32, 16});
constexpr int TWICE = 2;
constexpr int THIRD_COLOR = 2;
constexpr int THIRD_PLAYER = 3;
/** @brief Three steps of the beacons, and the fourth beacon of the second
 * bay: 19 + 3 × 9 in, 47 down. */
constexpr rtype::engine::Duration THREE_BEACON_STEPS =
    std::chrono::milliseconds(600);
constexpr sf::IntRect FOURTH_BEACON({180, 97}, {2, 2});
constexpr sf::Vector2i BEACON_SQUARE(2, 2);
constexpr const char* NICKNAME = "CAPTAIN_STARDUST";

}  // namespace rtype::client::testing
