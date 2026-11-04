#include "TickLoopFixtures.hpp"
#include "ServerTestConstants.hpp"
#include "SimulatedClock.hpp"
#include "TickLoop.hpp"
#include "TimeConstants.hpp"

void stallFor(SimulatedClock& clock, rtype::server::TickLoop& loop,
              int tickCount) {
  clock.advance(rtype::engine::SIMULATION_TICK_DURATION * tickCount +
                HALF_TICK);
  loop.runDueTicks();
}

rtype::engine::Duration droppedBy(int tickCount) {
  return rtype::engine::SIMULATION_TICK_DURATION *
             (tickCount - CAUGHT_UP_TICK_COUNT) +
         HALF_TICK;
}
