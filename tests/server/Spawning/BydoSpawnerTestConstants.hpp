#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "BydoKind.hpp"
#include "TicksToReach.hpp"
#include "TimeConstants.hpp"
#include "game/Waves/WaveTestConstants.hpp"

namespace rtype::server::testing {

/**
 * @brief How many ticks a game on game::testing::TEST_WAVE_CURVE runs until
 * its first Bydo appears, that tick included: the first pause, 241 ticks.
 */
constexpr std::size_t TICKS_TO_THE_FIRST_BYDO = game::testing::TICKS_OF_A_PAUSE;

/**
 * @brief How many ticks it runs until the second Bydo of its first wave
 * appears, that tick included: 361.
 */
constexpr std::size_t TICKS_TO_THE_SECOND_BYDO = engine::testing::ticksToReach(
    game::testing::TEST_WAVE_CURVE.wavePause +
    game::testing::TEST_WAVE_CURVE.firstWave.spawnInterval);

/**
 * @brief The kinds the Bydos of the spawner tests take in turn: today's
 * Drifter then Weaver, and a cycle that starts with the Weaver.
 */
constexpr std::array TEST_ENEMY_CYCLE{game::BydoKind::Drifter,
                                      game::BydoKind::Weaver};
constexpr std::array WEAVER_FIRST_CYCLE{game::BydoKind::Weaver,
                                        game::BydoKind::Drifter};

/**
 * @brief The seed the spawner tests start from, and another one, which gives
 * other heights.
 */
constexpr std::uint32_t SPAWNER_SEED = 42;
constexpr std::uint32_t OTHER_SPAWNER_SEED = 43;

/**
 * @brief Twenty seconds of ticks: more than a pause, and more than the time a
 * Bydo takes to cross the playfield, about 8.5 seconds.
 */
constexpr std::size_t TWENTY_SECONDS_OF_TICKS =
    engine::wholeTicksIn(std::chrono::seconds(20));

/**
 * @brief What the line the spawner writes at the start of the first wave of
 * game::testing::TEST_WAVE_CURVE holds: 3 Bydos, one every 2 seconds.
 */
constexpr std::string_view FIRST_WAVE_LINE_TEXT =
    "wave 1: 3 Bydos, one every 2000 ms";

}  // namespace rtype::server::testing
