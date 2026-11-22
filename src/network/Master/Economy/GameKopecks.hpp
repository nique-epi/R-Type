#pragma once

#include <cstdint>
#include "GameGain.hpp"

namespace rtype::network {

/** @returns @p amount, MAXIMUM_KOPECKS at most: a balance after a gain, as
 * boundedKopecks(std::uint64_t{balance} + gain). */
[[nodiscard]] std::uint32_t boundedKopecks(std::uint64_t amount);

/** @returns What @p gain pays: its bounty, and one kopeck per
 * SCORE_POINTS_PER_KOPECK points of its score, MAXIMUM_KOPECKS at most. */
[[nodiscard]] std::uint32_t kopecksFor(GameGain gain);

}  // namespace rtype::network
