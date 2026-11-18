#pragma once

#include <cstdint>

namespace rtype::network {

/** @returns @p amount, MAXIMUM_KOPECKS at most: a balance after a gain, as
 * boundedKopecks(std::uint64_t{balance} + gain). */
[[nodiscard]] std::uint32_t boundedKopecks(std::uint64_t amount);

/** @returns What a final score of @p score pays: one kopeck per
 * SCORE_POINTS_PER_KOPECK points, MAXIMUM_KOPECKS at most. */
[[nodiscard]] std::uint32_t kopecksFor(std::uint32_t score);

/** @returns @p kopecks and VICTORY_BONUS_PERCENT percent of it more, rounded
 * down, MAXIMUM_KOPECKS at most. */
[[nodiscard]] std::uint32_t withVictoryBonus(std::uint32_t kopecks);

}  // namespace rtype::network
