#pragma once

#include "BossHealthScaling.hpp"

namespace rtype::game {

/** @returns The points of @p scaling scaled for its players (at least one
 * counts), its health percent and its threat: each player past the first adds
 * BOSS_HEALTH_PER_EXTRA_PLAYER percent, then the whole is taken at the
 * health percent, then raised by the share of BOSS_HEALTH_AT_THE_MOST_THREAT
 * percent its threat gives, each step rounded down, never under 1. */
[[nodiscard]] int scaledBossHealth(const BossHealthScaling& scaling);

}  // namespace rtype::game
