#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::client {

/** @brief What a game pays its player: one kopeck per SCORE_POINTS_PER_KOPECK
 * points of the final score, and VICTORY_BONUS_PERCENT percent of that more
 * once the squadron won; a part of a kopeck is lost. */
constexpr std::uint32_t SCORE_POINTS_PER_KOPECK = 10;
constexpr std::uint64_t VICTORY_BONUS_PERCENT = 25;
constexpr std::uint64_t WHOLE_PERCENT_OF_A_GAIN = 100;

/** @brief How an amount is written: its digits in groups of
 * KOPECKS_DIGIT_GROUP from the right, KOPECKS_GROUP_SEPARATOR between two. */
constexpr std::size_t KOPECKS_DIGIT_GROUP = 3;
constexpr char KOPECKS_GROUP_SEPARATOR = ' ';

/** @brief The price of an item the shop does not sell. */
constexpr std::uint32_t NOT_FOR_SALE = 0;

}  // namespace rtype::client
