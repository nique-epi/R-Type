#pragma once

#include "Position.hpp"

namespace rtype::game {

/**
 * @brief Where a missile fired by the Bydo centered on @p bydo appears: on the
 * row of the Bydo, its right side against the left side of the Bydo, so it
 * leaves from the front of a Bydo, which flies left.
 */
[[nodiscard]] Position enemyMissileStartPosition(Position bydo);

}  // namespace rtype::game
