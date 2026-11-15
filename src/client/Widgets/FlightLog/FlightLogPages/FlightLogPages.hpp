#pragma once

#include "MissionLogLayout.hpp"
#include "RankingLayout.hpp"
#include "TextMetrics.hpp"

namespace rtype::client {

/** @returns Where the mission log lays its parts at the text size of
 * @p metrics: on its page of PAGE_SIZE as the book was drawn at the size the
 * fonts are drawn at; above it, on the wide page, WIDE_PAGE_WIDTH across the
 * open book, each part centered on it and moved down past the taller line
 * above, the caption of a polaroid centered under it. */
[[nodiscard]] MissionLogLayout missionLogLayoutOf(TextMetrics metrics);

/** @returns Where the ranking lays its parts at the text size of @p metrics:
 * on its page of PAGE_SIZE as drawn; above it, on the wide page, each row on
 * two lines, the name on the first, the marker, YOU, the leader and the score
 * on the second. */
[[nodiscard]] RankingLayout rankingLayoutOf(TextMetrics metrics);

}  // namespace rtype::client
