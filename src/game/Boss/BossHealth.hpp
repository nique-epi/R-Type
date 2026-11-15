#pragma once

#include "BossHealthScaling.hpp"

namespace rtype::game {

/** @returns The points of @p scaling scaled for its players (at least one
 * counts) and its health percent: each player past the first adds
 * BOSS_HEALTH_PER_EXTRA_PLAYER percent, then the whole is taken at the
 * health percent, rounded down, never under 1. */
[[nodiscard]] int scaledBossHealth(const BossHealthScaling& scaling);

}  // namespace rtype::game
