#pragma once

#include <chrono>
#include "BydoWave.hpp"
#include "WaveCurve.hpp"

namespace rtype::game::testing {

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
    .wavePause = std::chrono::seconds(4)};

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

}  // namespace rtype::game::testing
