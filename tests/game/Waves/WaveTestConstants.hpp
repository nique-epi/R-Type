#pragma once

#include <chrono>
#include <cstddef>
#include "BydoWave.hpp"
#include "TicksToReach.hpp"
#include "TimeConstants.hpp"
#include "WaveCurve.hpp"

namespace rtype::game::testing {

/**
 * @brief The waves of the level of TEST_WAVE_CURVE: more than any test plays,
 * so the level never ends in a test that does not ask for it.
 */
constexpr std::size_t LONG_LEVEL_WAVES = 100;

/**
 * @brief The waves the tests play: 3 Bydos one every 2 seconds, then one more
 * Bydo and a quarter of a second closer per wave, down to one every second,
 * with a pause of 4 seconds before each wave. They are the normal difficulty
 * of the waves file when it was written, so the timings of the tests read like
 * a game, but no test reads the file for them.
 */
constexpr WaveCurve TEST_WAVE_CURVE{
    .firstWave =
        BydoWave{.bydoCount = 3, .spawnInterval = std::chrono::seconds(2)},
    .bydosAddedPerWave = 1,
    .spawnIntervalCutPerWave = std::chrono::milliseconds(250),
    .shortestSpawnInterval = std::chrono::seconds(1),
    .wavePause = std::chrono::seconds(4),
    .levelWaves = LONG_LEVEL_WAVES};

/**
 * @brief TEST_WAVE_CURVE with a level of one wave, and of two.
 */
constexpr WaveCurve ONE_WAVE_LEVEL_CURVE{
    .firstWave = TEST_WAVE_CURVE.firstWave,
    .bydosAddedPerWave = TEST_WAVE_CURVE.bydosAddedPerWave,
    .spawnIntervalCutPerWave = TEST_WAVE_CURVE.spawnIntervalCutPerWave,
    .shortestSpawnInterval = TEST_WAVE_CURVE.shortestSpawnInterval,
    .wavePause = TEST_WAVE_CURVE.wavePause,
    .levelWaves = 1};
constexpr WaveCurve TWO_WAVE_LEVEL_CURVE{
    .firstWave = TEST_WAVE_CURVE.firstWave,
    .bydosAddedPerWave = TEST_WAVE_CURVE.bydosAddedPerWave,
    .spawnIntervalCutPerWave = TEST_WAVE_CURVE.spawnIntervalCutPerWave,
    .shortestSpawnInterval = TEST_WAVE_CURVE.shortestSpawnInterval,
    .wavePause = TEST_WAVE_CURVE.wavePause,
    .levelWaves = 2};

/**
 * @brief The waves TEST_WAVE_CURVE brings after its first, worked out by hand:
 * 4 Bydos 1.75 seconds apart, then 6 at 1.25 seconds for the fourth wave, then
 * 7 and 8 one every second, the shortest time.
 */
constexpr BydoWave TEST_SECOND_WAVE{
    .bydoCount = 4, .spawnInterval = std::chrono::milliseconds(1750)};
constexpr BydoWave TEST_FOURTH_WAVE{
    .bydoCount = 6, .spawnInterval = std::chrono::milliseconds(1250)};
constexpr BydoWave TEST_FIFTH_WAVE{.bydoCount = 7,
                                   .spawnInterval = std::chrono::seconds(1)};
constexpr BydoWave TEST_SIXTH_WAVE{.bydoCount = 8,
                                   .spawnInterval = std::chrono::seconds(1)};

/**
 * @brief When the last Bydo of the first wave of TEST_WAVE_CURVE comes: its
 * pause of 4 seconds, then two intervals of 2 seconds, 8 seconds into the
 * game.
 */
constexpr engine::Duration END_OF_THE_FIRST_TEST_WAVE = std::chrono::seconds(8);

/**
 * @brief How many ticks a pause of TEST_WAVE_CURVE lasts, counting the tick it
 * starts on and the tick the Bydo after it is due on: 4 seconds are reached on
 * the 241st tick. A game starts with a pause, so it is also the tick of its
 * first Bydo.
 */
constexpr std::size_t TICKS_OF_A_PAUSE =
    engine::testing::ticksToReach(TEST_WAVE_CURVE.wavePause);

/**
 * @brief How many ticks run until every Bydo of the first wave is due, the
 * tick of its last Bydo included: the 481st.
 */
constexpr std::size_t TICKS_TO_THE_WHOLE_FIRST_WAVE =
    engine::testing::ticksToReach(END_OF_THE_FIRST_TEST_WAVE);

}  // namespace rtype::game::testing
