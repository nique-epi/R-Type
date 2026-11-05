#pragma once

#include <chrono>
#include <cstddef>
#include "Wave.hpp"
#include "WaveCurve.hpp"
#include "WaveTestConstants.hpp"

namespace rtype::game::testing {

/**
 * @brief A wave of TEST_WAVE_CURVE whose Bydos come 1.1 seconds apart, less
 * than a quarter of a second above its shortest time, and the wave after it,
 * one every second: a whole cut would bring them 0.85 seconds apart.
 */
constexpr Wave WAVE_JUST_ABOVE_THE_SHORTEST{
    .entityCount = 5, .spawnInterval = std::chrono::milliseconds(1100)};
constexpr Wave WAVE_AFTER_THE_ONE_JUST_ABOVE{
    .entityCount = 6, .spawnInterval = std::chrono::seconds(1)};

/**
 * @brief A curve that adds no Bydo and cuts nothing: every wave is its first.
 */
constexpr WaveCurve STEADY_CURVE{
    .firstWave =
        Wave{.entityCount = 3, .spawnInterval = std::chrono::seconds(2)},
    .entitiesAddedPerWave = 0,
    .spawnIntervalCutPerWave = std::chrono::milliseconds(0),
    .shortestSpawnInterval = std::chrono::seconds(2),
    .wavePause = std::chrono::seconds(4),
    .levelWaves = LONG_LEVEL_WAVES};

/**
 * @brief Curves that are TEST_WAVE_CURVE but for one value, each breaking one
 * condition of isPlayable: a first wave without Bydos, a level without a
 * wave, no time between two Bydos, a shortest time longer than the first one, a
 * negative cut, a negative pause.
 */
constexpr WaveCurve CURVE_WITHOUT_BYDOS{
    .firstWave =
        Wave{.entityCount = 0, .spawnInterval = std::chrono::seconds(2)},
    .entitiesAddedPerWave = 1,
    .spawnIntervalCutPerWave = std::chrono::milliseconds(250),
    .shortestSpawnInterval = std::chrono::seconds(1),
    .wavePause = std::chrono::seconds(4),
    .levelWaves = LONG_LEVEL_WAVES};
constexpr WaveCurve CURVE_WITHOUT_WAVES{
    .firstWave =
        Wave{.entityCount = 3, .spawnInterval = std::chrono::seconds(2)},
    .entitiesAddedPerWave = 1,
    .spawnIntervalCutPerWave = std::chrono::milliseconds(250),
    .shortestSpawnInterval = std::chrono::seconds(1),
    .wavePause = std::chrono::seconds(4),
    .levelWaves = 0};
constexpr WaveCurve CURVE_WITHOUT_TIME_BETWEEN_BYDOS{
    .firstWave =
        Wave{.entityCount = 3, .spawnInterval = std::chrono::seconds(2)},
    .entitiesAddedPerWave = 1,
    .spawnIntervalCutPerWave = std::chrono::milliseconds(250),
    .shortestSpawnInterval = std::chrono::seconds(0),
    .wavePause = std::chrono::seconds(4),
    .levelWaves = LONG_LEVEL_WAVES};
constexpr WaveCurve CURVE_THAT_SLOWS_DOWN{
    .firstWave =
        Wave{.entityCount = 3, .spawnInterval = std::chrono::seconds(2)},
    .entitiesAddedPerWave = 1,
    .spawnIntervalCutPerWave = std::chrono::milliseconds(250),
    .shortestSpawnInterval = std::chrono::milliseconds(2500),
    .wavePause = std::chrono::seconds(4),
    .levelWaves = LONG_LEVEL_WAVES};
constexpr WaveCurve CURVE_WITH_A_NEGATIVE_CUT{
    .firstWave =
        Wave{.entityCount = 3, .spawnInterval = std::chrono::seconds(2)},
    .entitiesAddedPerWave = 1,
    .spawnIntervalCutPerWave = std::chrono::milliseconds(-250),
    .shortestSpawnInterval = std::chrono::seconds(1),
    .wavePause = std::chrono::seconds(4),
    .levelWaves = LONG_LEVEL_WAVES};
constexpr WaveCurve CURVE_WITH_A_NEGATIVE_PAUSE{
    .firstWave =
        Wave{.entityCount = 3, .spawnInterval = std::chrono::seconds(2)},
    .entitiesAddedPerWave = 1,
    .spawnIntervalCutPerWave = std::chrono::milliseconds(250),
    .shortestSpawnInterval = std::chrono::seconds(1),
    .wavePause = std::chrono::seconds(-4),
    .levelWaves = LONG_LEVEL_WAVES};

/**
 * @brief How many waves the tests of a long game follow: far past the fifth,
 * from which the Bydos of TEST_WAVE_CURVE stop coming closer together.
 */
constexpr std::size_t MANY_WAVES = 50;

}  // namespace rtype::game::testing
