#pragma once

#include "Wave.hpp"
#include "WaveCurve.hpp"

namespace rtype::game {

/**
 * @returns The wave that comes after @p previous on @p curve: its
 * entitiesAddedPerWave more enemies, its spawnIntervalCutPerWave closer
 * together, but never closer than its shortestSpawnInterval.
 */
[[nodiscard]] Wave waveAfter(Wave previous, const WaveCurve& curve);

/**
 * @returns Whether every wave of @p curve holds an enemy or more, with a time
 * above zero between two of them that never grows: true when its first wave
 * holds at least one enemy, its shortest interval is above zero and no longer
 * than the interval of its first wave, and neither its cut nor its pause is
 * negative, and its level holds at least one wave; false otherwise.
 */
[[nodiscard]] bool isPlayable(const WaveCurve& curve);

}  // namespace rtype::game
