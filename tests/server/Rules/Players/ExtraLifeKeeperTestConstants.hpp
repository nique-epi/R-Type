#pragma once

#include <cstdint>

namespace rtype::server::testing {

/** @brief A kill that brings a score of 0 to the first step of the extra
 * lives, 30000, and one that leaves it far under. */
constexpr std::uint32_t POINTS_TO_THE_FIRST_STEP = 30000;
constexpr std::uint32_t POINTS_OF_A_DRIFTER = 100;

/** @brief The lives of a new ship, the same plus the one won, the most a
 * player may hold, and none. */
constexpr std::uint8_t LIVES_OF_A_NEW_SHIP = 3;
constexpr std::uint8_t LIVES_WITH_ONE_WON = 4;
constexpr std::uint8_t MOST_LIVES = 9;
constexpr std::uint8_t NO_LIVES = 0;

}  // namespace rtype::server::testing
