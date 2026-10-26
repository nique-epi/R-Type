#pragma once

#include <chrono>
#include <cstddef>
#include "TicksToReach.hpp"
#include "TimeConstants.hpp"
#include "WaveTestConstants.hpp"

namespace rtype::game::testing {

/** @brief The numbers of the first wave of a game and of the second. */
constexpr std::size_t FIRST_WAVE_NUMBER_IN_THE_LOG = 1;
constexpr std::size_t SECOND_WAVE_NUMBER_IN_THE_LOG = 2;

/**
 * @brief How many ticks after a Bydo of the first wave the next one is due: 2
 * seconds, 120 ticks. The Bydos of a wave are due at the end of its pause plus
 * whole intervals, so the ticks are counted from the end of the pause.
 */
constexpr std::size_t TICKS_BETWEEN_TWO_BYDOS_OF_THE_FIRST_WAVE =
    engine::testing::ticksToReach(TEST_WAVE_CURVE.wavePause +
                                  TEST_WAVE_CURVE.firstWave.spawnInterval) -
    TICKS_OF_A_PAUSE;

/** @brief The same for the second wave: 1.75 seconds, 105 ticks. */
constexpr std::size_t TICKS_BETWEEN_TWO_BYDOS_OF_THE_SECOND_WAVE =
    engine::testing::ticksToReach(TEST_WAVE_CURVE.wavePause +
                                  TEST_SECOND_WAVE.spawnInterval) -
    TICKS_OF_A_PAUSE;

/** @brief How many Bydos the world holds, as a test tells the director. */
constexpr std::size_t NO_BYDO_IN_PLAY = 0;
constexpr std::size_t ONE_BYDO_IN_PLAY = 1;

/**
 * @brief Half a second of ticks, for which a Bydo stays in play once its wave
 * appeared.
 */
constexpr std::size_t HALF_A_SECOND_OF_TICKS =
    engine::wholeTicksIn(std::chrono::milliseconds(500));

/**
 * @brief A minute of ticks: longer than any wait for a Bydo the tests expect,
 * so it is also the most a test waits for one before it fails.
 */
constexpr std::size_t A_MINUTE_OF_TICKS =
    engine::wholeTicksIn(std::chrono::minutes(1));

}  // namespace rtype::game::testing
