#include "ServerClockFixtures.hpp"
#include <cstdint>
#include "ClockSample.hpp"
#include "ServerClock.hpp"
#include "ServerClockTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine::testing {

ClockSample symmetricSample(std::uint32_t tick, Duration tickZero,
                            Duration roundTrip) {
  const Duration sentAt = tickZero + SIMULATION_TICK_DURATION * tick;
  return ClockSample{.receivedAt = sentAt + roundTrip / WAYS_OF_A_PATH,
                     .roundTrip = roundTrip,
                     .tick = tick};
}

void addTheBestSampleThenWorseOnes(ServerClock& clock) {
  clock.add(symmetricSample(TEN_SECONDS_IN, Duration::zero(), BEST_ROUND_TRIP));
  for (std::uint32_t sample = 1; sample <= SAMPLES_BEFORE_THE_BEST_LEAVES;
       ++sample) {
    clock.add(symmetricSample(TEN_SECONDS_IN + sample, FOUR_MILLISECONDS,
                              USUAL_ROUND_TRIP));
  }
}

}  // namespace rtype::engine::testing
