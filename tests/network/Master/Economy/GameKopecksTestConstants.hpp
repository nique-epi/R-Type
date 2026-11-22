#pragma once

#include <cstdint>

namespace rtype::network::testing {

/** @brief A score of 12450 pays 249 kopecks, 50 points a kopeck; one of 49
 * pays none. */
constexpr std::uint32_t DESIGN_SCORE = 12450;
constexpr std::uint32_t KOPECKS_OF_THE_DESIGN_SCORE = 249;
constexpr std::uint32_t SCORE_UNDER_ONE_KOPECK = 49;
/** @brief The bounty of the Necromancer and of the Pit cleared, 350 and 300,
 * and what it pays with the design score: 650 and 249, 899. */
constexpr std::uint32_t BOUNTY_OF_THE_PIT = 650;
constexpr std::uint32_t KOPECKS_OF_THE_PIT = 899;
/** @brief The largest balance, and the largest score and bounty of the
 * protocol. */
constexpr std::uint32_t LARGEST_BALANCE = 999'999;
constexpr std::uint32_t LARGEST_SCORE = 4'294'967'295;
constexpr std::uint32_t LARGEST_BOUNTY = 4'294'967'295;

}  // namespace rtype::network::testing
