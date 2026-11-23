#pragma once

#include "TeamScaling.hpp"

namespace rtype::game {

/**
 * @returns The percent a team of @p scaling is worth: WHOLE_PERCENT, raised by
 * its percent per extra player for each player past the first; no player
 * counts as one. The health of a boss takes 75 % per player, the health of
 * the other enemies 40 %, and their guns -10 %.
 */
[[nodiscard]] int teamPercent(TeamScaling scaling);

}  // namespace rtype::game
