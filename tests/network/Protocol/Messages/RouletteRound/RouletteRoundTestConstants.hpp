#pragma once

#include <cstdint>

namespace rtype::network::testing {

/** @brief Round 0x0304, with 0x0506 ticks left, two numbers of distinct bytes
 * as the protocol page writes them. */
constexpr std::uint16_t SOME_ROUND_OF_DISTINCT_BYTES = 0x0304;
constexpr std::uint16_t SOME_TICKS_LEFT = 0x0506;

/** @brief The number 17 (0x11) drawn, a stake of 500 chips (0x01F4) on it,
 * and the payout of a straight bet: 36 times the stake, 18000 (0x4650). */
constexpr std::uint8_t DRAWN_SEVENTEEN = 17;
constexpr std::uint32_t STAKE_OF_FIVE_HUNDRED = 500;
constexpr std::uint32_t STRAIGHT_PAYOUT_OF_FIVE_HUNDRED = 18000;

/** @brief The seat numbers of two players at the table. */
constexpr std::uint8_t FIRST_SEAT_NUMBER = 1;
constexpr std::uint8_t SECOND_SEAT_NUMBER = 2;

/** @brief One past the highest number of the wheel, and one past the last
 * phase. */
constexpr std::uint8_t WINNING_NUMBER_PAST_THE_WHEEL = 37;
constexpr std::uint8_t PHASE_PAST_THE_LAST = 3;

}  // namespace rtype::network::testing
