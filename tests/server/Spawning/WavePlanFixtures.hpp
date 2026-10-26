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

}  // namespace rtype::server::testing
