#pragma once

#include "BydoWave.hpp"
#include "WaveCurve.hpp"

namespace rtype::game {

/**
 * @returns The wave that comes after @p previous on @p curve: its
 * bydosAddedPerWave more Bydos, its spawnIntervalCutPerWave closer together,
 * but never closer than its shortestSpawnInterval.
 */
[[nodiscard]] BydoWave bydoWaveAfter(BydoWave previous, const WaveCurve& curve);

/**
 * @returns Whether every wave of @p curve holds a Bydo or more, with a time
 * above zero between two of them that never grows: true when its first wave
 * holds at least one Bydo, its shortest interval is above zero and no longer
 * than the interval of its first wave, and neither its cut nor its pause is
 * negative; false otherwise.
 */
[[nodiscard]] bool isPlayable(const WaveCurve& curve);

}  // namespace rtype::game
