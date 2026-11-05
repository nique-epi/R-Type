#pragma once

#include <cstddef>
#include "BydoWave.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @brief How the waves of one difficulty grow: the first wave, then what each
 * next wave adds and takes off, the shortest time it may leave between two
 * Bydos, the pause before each wave, in simulation time, and how many
 * waves the level of the difficulty holds.
 */
struct WaveCurve {
  BydoWave firstWave;
  std::size_t bydosAddedPerWave{};
  engine::Duration spawnIntervalCutPerWave{};
  engine::Duration shortestSpawnInterval{};
  engine::Duration wavePause{};
  std::size_t levelWaves{};

  bool operator==(const WaveCurve&) const = default;
};

}  // namespace rtype::game
