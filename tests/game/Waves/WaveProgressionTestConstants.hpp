#pragma once

#include <chrono>
#include <cstddef>
#include "BydoWave.hpp"
#include "WaveCurve.hpp"

namespace rtype::game::testing {

/**
 * @brief A wave of TEST_WAVE_CURVE whose Bydos come 1.1 seconds apart, less
 * than a quarter of a second above its shortest time, and the wave after it,
 * one every second: a whole cut would bring them 0.85 seconds apart.
 */
constexpr BydoWave WAVE_JUST_ABOVE_THE_SHORTEST{
    .bydoCount = 5, .spawnInterval = std::chrono::milliseconds(1100)};
constexpr BydoWave WAVE_AFTER_THE_ONE_JUST_ABOVE{
    .bydoCount = 6, .spawnInterval = std::chrono::seconds(1)};

/**
 * @brief A curve that adds no Bydo and cuts nothing: every wave is its first.
 */
constexpr WaveCurve STEADY_CURVE{
    .firstWave =
        BydoWave{.bydoCount = 3, .spawnInterval = std::chrono::seconds(2)},
    .bydosAddedPerWave = 0,
    .spawnIntervalCutPerWave = std::chrono::milliseconds(0),
    .shortestSpawnInterval = std::chrono::seconds(2),
    .wavePause = std::chrono::seconds(4)};

/**
 * @brief Curves that are TEST_WAVE_CURVE but for one value, each breaking one
 * condition of isPlayable: a first wave without Bydos, no time between two
 * Bydos, a shortest time longer than the first one, a negative cut, a
 * negative pause.
 */
constexpr WaveCurve CURVE_WITHOUT_BYDOS{
    .firstWave =
        BydoWave{.bydoCount = 0, .spawnInterval = std::chrono::seconds(2)},
    .bydosAddedPerWave = 1,
    .spawnIntervalCutPerWave = std::chrono::milliseconds(250),
    .shortestSpawnInterval = std::chrono::seconds(1),
    .wavePause = std::chrono::seconds(4)};
constexpr WaveCurve CURVE_WITHOUT_TIME_BETWEEN_BYDOS{
    .firstWave =
        BydoWave{.bydoCount = 3, .spawnInterval = std::chrono::seconds(2)},
    .bydosAddedPerWave = 1,
    .spawnIntervalCutPerWave = std::chrono::milliseconds(250),
    .shortestSpawnInterval = std::chrono::seconds(0),
    .wavePause = std::chrono::seconds(4)};
constexpr WaveCurve CURVE_THAT_SLOWS_DOWN{
    .firstWave =
        BydoWave{.bydoCount = 3, .spawnInterval = std::chrono::seconds(2)},
    .bydosAddedPerWave = 1,
    .spawnIntervalCutPerWave = std::chrono::milliseconds(250),
    .shortestSpawnInterval = std::chrono::milliseconds(2500),
    .wavePause = std::chrono::seconds(4)};
constexpr WaveCurve CURVE_WITH_A_NEGATIVE_CUT{
    .firstWave =
        BydoWave{.bydoCount = 3, .spawnInterval = std::chrono::seconds(2)},
    .bydosAddedPerWave = 1,
    .spawnIntervalCutPerWave = std::chrono::milliseconds(-250),
    .shortestSpawnInterval = std::chrono::seconds(1),
    .wavePause = std::chrono::seconds(4)};
constexpr WaveCurve CURVE_WITH_A_NEGATIVE_PAUSE{
    .firstWave =
        BydoWave{.bydoCount = 3, .spawnInterval = std::chrono::seconds(2)},
    .bydosAddedPerWave = 1,
    .spawnIntervalCutPerWave = std::chrono::milliseconds(250),
    .shortestSpawnInterval = std::chrono::seconds(1),
    .wavePause = std::chrono::seconds(-4)};

/**
 * @brief How many waves the tests of a long game follow: far past the fifth,
 * from which the Bydos of TEST_WAVE_CURVE stop coming closer together.
 */
constexpr std::size_t MANY_WAVES = 50;

}  // namespace rtype::game::testing
