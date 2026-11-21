#pragma once

#include "TeamScaling.hpp"

namespace rtype::game {

/**
 * @returns The percent a team of @p scaling is worth: WHOLE_PERCENT, raised by
 * its percent per extra player for each player past the first; no player
 * counts as one. Bosses take 25 % per player, the other enemies 15 %.
 */
[[nodiscard]] int teamPercent(TeamScaling scaling);

}  // namespace rtype::game
