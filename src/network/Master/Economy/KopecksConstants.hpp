#pragma once

#include <cstdint>

namespace rtype::network {

/** @brief What a game pays its player: its bounty, and one kopeck per
 * SCORE_POINTS_PER_KOPECK points of its final score, a part of a kopeck lost.
 * The bounty carries most of it, so a squad earns as much as a lone pilot and
 * the last hit on a boss is worth no more than the others. The master credits
 * it and the client shows it, so both compute it here. */
constexpr std::uint32_t SCORE_POINTS_PER_KOPECK = 50;

/** @brief The price of an item the shop does not sell. */
constexpr std::uint32_t NOT_FOR_SALE = 0;

/** @brief The largest balance of kopecks a player holds, six digits, which
 * the frame of the title of the client is as wide as. */
constexpr std::uint32_t MAXIMUM_KOPECKS = 999'999;

/** @brief The largest number of chips a player holds: the largest balance of
 * kopecks, so a whole balance can be changed either way at the cashier, one
 * for one. */
constexpr std::uint32_t MAXIMUM_CHIPS = MAXIMUM_KOPECKS;

}  // namespace rtype::network
