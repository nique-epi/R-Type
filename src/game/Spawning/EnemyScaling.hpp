#pragma once

#include <cstddef>
#include "EnemyStrength.hpp"

namespace rtype::game {

/**
 * @returns The strength the enemies of a game are made at: @p difficulty, the
 * percents of the difficulty it is played at, with the health raised by
 * ENEMY_HEALTH_PER_EXTRA_PLAYER percent for each of @p players past the
 * first; no player counts as one. The gun is the difficulty's alone: more
 * players already face more shots, since each enemy fires at the nearest.
 */
[[nodiscard]] EnemyStrength scaledEnemyStrength(EnemyStrength difficulty,
                                                std::size_t players);

}  // namespace rtype::game
