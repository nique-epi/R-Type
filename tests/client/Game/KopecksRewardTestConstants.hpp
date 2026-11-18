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

}  // namespace rtype::client::testing
