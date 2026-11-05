#pragma once

#include <span>
#include "BydoKind.hpp"
#include "WavePlan.hpp"

namespace rtype::server::testing {

/**
 * @returns The waves of game::testing::TEST_WAVE_CURVE, their Bydos taking the
 * kinds of @p enemies in turn.
 */
[[nodiscard]] WavePlan wavePlanOf(std::span<const game::BydoKind> enemies);

/** @returns The waves the server tests play: wavePlanOf(TEST_ENEMY_CYCLE). */
[[nodiscard]] WavePlan testWavePlan();

/**
 * @returns The same waves with a level of one wave
 * (game::testing::ONE_WAVE_LEVEL_CURVE): the game is won once the first wave
 * is over.
 */
[[nodiscard]] WavePlan oneWaveLevelWavePlan();

}  // namespace rtype::server::testing
