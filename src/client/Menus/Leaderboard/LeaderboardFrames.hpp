#pragma once

#include "LeaderboardFrame.hpp"
#include "TextMetrics.hpp"

namespace rtype::client {

/**
 * @returns The frame of the leaderboard at @p metrics: as the leaderboard was
 * drawn at the size the fonts are drawn at; above it, a panel across the room
 * between the safe margins, under tabs grown with the text, its rows on two
 * lines, the summary of the filter on a row of its own over the chips.
 */
[[nodiscard]] LeaderboardFrame leaderboardFrameOf(TextMetrics metrics);

}  // namespace rtype::client
