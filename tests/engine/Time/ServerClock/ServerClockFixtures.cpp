#include "ServerClockFixtures.hpp"
#include <cstdint>
#include "ClockSample.hpp"
#include "ServerClock.hpp"
#include "ServerClockTestConstants.hpp"
#include "SymmetricPath.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine::testing {

ClockSample symmetricSample(std::uint32_t tick, const SymmetricPath& path) {
  const Duration sentAt = path.tickZero + SIMULATION_TICK_DURATION * tick;
  return ClockSample{.receivedAt = sentAt + path.roundTrip / WAYS_OF_A_PATH,
                     .roundTrip = path.roundTrip,
                     .tick = tick};
}

void addTheBestSampleThenWorseOnes(ServerClock& clock) {
  clock.add(symmetricSample(TEN_SECONDS_IN,
                            SymmetricPath{.tickZero = Duration::zero(),
                                          .roundTrip = BEST_ROUND_TRIP}));
  for (std::uint32_t sample = 1; sample <= SAMPLES_BEFORE_THE_BEST_LEAVES;
       ++sample) {
    clock.add(symmetricSample(TEN_SECONDS_IN + sample,
                              SymmetricPath{.tickZero = FOUR_MILLISECONDS,
                                            .roundTrip = USUAL_ROUND_TRIP}));
  }
}

}  // namespace rtype::engine::testing
