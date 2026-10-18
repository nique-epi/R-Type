#pragma once

#include "Position.hpp"

namespace rtype::game {

/**
 * @brief Where a Bydo appears: just past the right edge of the playfield, so
 * it is off screen, at a height picked by @p randomUnit.
 *
 * @p randomUnit 0 gives the highest row and a value just under 1 the lowest;
 * the rows stay BYDO_SPAWN_VERTICAL_MARGIN away from the top and the bottom,
 * so the whole Bydo is inside the height once it has come in.
 *
 * @param randomUnit From 0 up to, but not including, 1.
 * @throws InvalidRandomUnitException when @p randomUnit is outside that range
 * or is not a number.
 */
[[nodiscard]] Position bydoSpawnPosition(float randomUnit);

}  // namespace rtype::game
