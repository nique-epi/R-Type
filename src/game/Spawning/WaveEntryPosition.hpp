#pragma once

#include "Position.hpp"

namespace rtype::game {

/**
 * @brief Where an enemy appears: just past the right edge of the playfield, so
 * it is off screen, at a height picked by @p randomUnit.
 *
 * @p randomUnit 0 gives the highest row and a value just under 1 the lowest;
 * the rows stay ENEMY_ROW_MARGIN away from the top of the playfield and from
 * the status band along its bottom, so the whole enemy stays inside the area
 * of play once it has come in, even at the top and the bottom of a wave.
 *
 * @param randomUnit From 0 up to, but not including, 1.
 * @throws InvalidRandomUnitException when @p randomUnit is outside that range
 * or is not a number.
 */
[[nodiscard]] Position waveEntryPosition(float randomUnit);

}  // namespace rtype::game
