#pragma once

#include <cstdint>

namespace rtype::client::testing {

/** @brief The final score and bounty of the pilot of this client, and what
 * they pay: 249 kopecks for 12450 points at 50 a kopeck, and the bounty of
 * 650, 899. */
constexpr std::uint32_t OWN_FINAL_SCORE = 12450;
constexpr std::uint32_t OWN_BOUNTY = 650;
constexpr std::uint32_t OWN_GAIN = 899;
/** @brief The score and bounty of another pilot, far larger, which pay this
 * client nothing. */
constexpr std::uint32_t ANOTHER_PILOT_SCORE = 90000;
constexpr std::uint32_t ANOTHER_PILOT_BOUNTY = 1000;

}  // namespace rtype::client::testing
