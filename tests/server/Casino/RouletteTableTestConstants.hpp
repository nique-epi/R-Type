#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::server::testing {

/** @brief The seats of the players of a test, their index in the roster. */
constexpr std::size_t FIRST_TABLE_SEAT = 0;
constexpr std::size_t SECOND_TABLE_SEAT = 1;

/** @brief The number the scripted draw gives, and a number next to it. */
constexpr std::uint8_t SCRIPTED_NUMBER = 17;
constexpr std::uint8_t NEIGHBOUR_NUMBER = 18;

/** @brief A stake of 100 chips, and what it returns straight on the number
 * drawn: 36 times, 3600. */
constexpr std::uint32_t HUNDRED_CHIPS = 100;
constexpr std::uint32_t STRAIGHT_RETURN_OF_A_HUNDRED = 3600;

/** @brief Two bets that add up to the cap of a round, and one chip more. */
constexpr std::uint32_t HALF_THE_CAP = 2500;
constexpr std::uint32_t ONE_CHIP_MORE = 2501;

/** @brief A few ticks past a phase, to show the clock does not move. */
constexpr std::size_t A_FEW_TICKS = 5;

}  // namespace rtype::server::testing
