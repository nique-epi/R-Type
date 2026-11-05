#pragma once

#include <cstdint>
#include "ClockSample.hpp"
#include "ServerClock.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine::testing {

/**
 * @returns The sample of a heartbeat sent at @p tick by a server whose tick 0
 * stood at @p tickZero on the clock of the client, over a path whose two ways
 * last the same, half of @p roundTrip: it arrived half a round trip after it
 * was sent.
 */
ClockSample symmetricSample(std::uint32_t tick, Duration tickZero,
                            Duration roundTrip);

/**
 * @brief Gives @p clock a sample with the best round trip, then
 * SAMPLES_BEFORE_THE_BEST_LEAVES with a worse one that all put tick 0 four
 * milliseconds later: the best sample is then the oldest of a window one
 * short of full.
 */
void addTheBestSampleThenWorseOnes(ServerClock& clock);

}  // namespace rtype::engine::testing
