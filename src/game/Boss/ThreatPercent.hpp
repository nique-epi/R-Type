#pragma once

#include "ThreatScaling.hpp"

namespace rtype::game {

/**
 * @returns The percent a threat of @p scaling is worth: WHOLE_PERCENT, raised
 * by the share of its percent at the most its threat gives, rounded toward
 * zero. The health of a boss takes 150 % at the most, the health of the
 * other enemies 50 %, and their guns -23 %.
 */
[[nodiscard]] int threatenedPercent(ThreatScaling scaling);

}  // namespace rtype::game
