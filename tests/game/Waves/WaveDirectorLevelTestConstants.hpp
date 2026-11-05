#pragma once

#include <chrono>
#include <cstddef>
#include "TicksToReach.hpp"
#include "WaveTestConstants.hpp"

namespace rtype::game::testing {

/**
 * @brief When the last Bydo of the second wave of TEST_WAVE_CURVE comes,
 * counted from the step that finds the sky clear after the first wave: its
 * pause of 4 seconds, then three intervals of 1.75 seconds, 9.25 seconds in.
 */
constexpr engine::Duration END_OF_THE_SECOND_TEST_WAVE =
    TEST_WAVE_CURVE.wavePause + 3 * TEST_SECOND_WAVE.spawnInterval;

/**
 * @brief How many ticks run until every Bydo of the second wave is due.
 */
constexpr std::size_t TICKS_TO_THE_WHOLE_SECOND_WAVE =
    engine::testing::ticksToReach(END_OF_THE_SECOND_TEST_WAVE);

}  // namespace rtype::game::testing
