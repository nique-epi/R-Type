#pragma once

#include <cstdint>

namespace rtype::client::testing {

/** @brief The final score of the pilot of this client, and what it pays: 1245
 * kopecks, a quarter more, rounded down, after a victory. */
constexpr std::uint32_t OWN_FINAL_SCORE = 12450;
constexpr std::uint32_t OWN_GAIN_AFTER_A_DEFEAT = 1245;
constexpr std::uint32_t OWN_GAIN_AFTER_A_VICTORY = 1556;
/** @brief The score of another pilot, far larger, which pays this client
 * nothing. */
constexpr std::uint32_t ANOTHER_PILOT_SCORE = 90000;
/** @brief A balance before the game, and after the gain of a defeat. */
constexpr std::uint32_t BALANCE_BEFORE_THE_GAME = 300;
constexpr std::uint32_t BALANCE_AFTER_THE_GAME = 1545;
/** @brief A balance one kopeck under the largest: a gain stops at 999999. */
constexpr std::uint32_t BALANCE_UNDER_THE_LARGEST = 999'998;
constexpr std::uint32_t THE_LARGEST_BALANCE = 999'999;

}  // namespace rtype::client::testing
