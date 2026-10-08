#include "TickLoop.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <functional>
#include <ratio>
#include <stop_token>
#include <string>
#include <thread>
#include <utility>
#include "FineTimerResolution.hpp"
#include "IClock.hpp"
#include "ServerConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

double inMilliseconds(engine::Duration duration) {
  return std::chrono::duration<double, std::milli>(duration).count();
}

}  // namespace

TickLoop::TickLoop(const engine::IClock& clock, std::function<void()> tick)
    : clock_(&clock),
      timestep_(clock, engine::SIMULATION_TICK_DURATION),
      tick_(std::move(tick)),
      nextTickDue_(clock.now() + timestep_.timeUntilNextTick()),
      logger_(std::string{SIMULATION_LOGGER_NAME}) {}

std::size_t TickLoop::runDueTicks() {
  const std::size_t dueTicks = timestep_.consumeTicks();
  nextTickDue_ = clock_->now() + timestep_.timeUntilNextTick();
  for (std::size_t tickIndex = 0; tickIndex < dueTicks; ++tickIndex) {
    const auto ticksBeforeNext =
        static_cast<engine::Duration::rep>(dueTicks - tickIndex);
    const engine::Duration due =
        nextTickDue_ - engine::SIMULATION_TICK_DURATION * ticksBeforeNext;
    record(clock_->now() - due);
    tick_();
  }
  return dueTicks;
}

void TickLoop::run(const std::stop_token& stop) {
  const FineTimerResolution fineTimers;
  if (!fineTimers.isGranted()) {
    logger_.warn("the system refused a timer resolution of ",
                 FINE_TIMER_RESOLUTION_MILLISECONDS,
                 " ms: ticks will start late, some of them in pairs");
  }
  while (!stop.stop_requested()) {
    runDueTicks();
    if (lateness_.tickCount >= TICKS_PER_LATENESS_REPORT) {
      reportLateness();
    }
    const engine::Duration untilNextTick = nextTickDue_ - clock_->now();
    if (untilNextTick > engine::Duration::zero()) {
      std::this_thread::sleep_for(untilNextTick);
    }
  }
}

TickLoop::Lateness TickLoop::takeLateness() {
  return std::exchange(lateness_, Lateness{});
}

void TickLoop::record(engine::Duration lateness) {
  ++lateness_.tickCount;
  lateness_.total += lateness;
  lateness_.maximum = std::max(lateness_.maximum, lateness);
}

void TickLoop::reportLateness() {
  const Lateness lateness = takeLateness();
  const engine::Duration average =
      lateness.total / static_cast<engine::Duration::rep>(lateness.tickCount);
  logger_.debug(lateness.tickCount, " ticks started on average ",
                inMilliseconds(average), " ms late, at most ",
                inMilliseconds(lateness.maximum), " ms");
}

}  // namespace rtype::server
