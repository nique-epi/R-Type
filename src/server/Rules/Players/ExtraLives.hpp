#pragma once

#include <cstdint>
#include "ScoreChanged.hpp"

namespace rtype::server {

/**
 * @returns How many extra lives @p change wins: the steps
 * FIRST_EXTRA_LIFE_SCORE, then each EXTRA_LIFE_SCORE_STEP past it, that lie
 * above its score before and at or below its score after. 0 when the score
 * did not grow.
 */
[[nodiscard]] std::uint32_t extraLivesWonBy(const game::ScoreChanged& change);

}  // namespace rtype::server
