#pragma once

#include <span>
#include <string_view>
#include "WavePlan.hpp"

namespace rtype::server::testing {

/**
 * @returns The waves of game::testing::TEST_WAVE_CURVE, their enemies taking
 * the sheets @p enemies names in turn.
 */
[[nodiscard]] WavePlan wavePlanOf(std::span<const std::string_view> enemies);

/**
 * @returns The waves the server tests play, their look items taking the
 * sheets @p lookItems names in turn.
 */
[[nodiscard]] WavePlan wavePlanWithLookItems(
    std::span<const std::string_view> lookItems);

/** @returns The waves the server tests play: wavePlanOf(TEST_ENEMY_CYCLE). */
[[nodiscard]] WavePlan testWavePlan();

/**
 * @returns The same waves with a level of one wave
 * (game::testing::ONE_WAVE_LEVEL_CURVE): the game is won once the first wave
 * is over.
 */
[[nodiscard]] WavePlan oneWaveLevelWavePlan();

}  // namespace rtype::server::testing
