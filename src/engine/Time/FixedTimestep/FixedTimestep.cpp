#include "FixedTimestep.hpp"
#include <algorithm>
#include <cstddef>
#include "EngineException.hpp"
#include "IClock.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

FixedTimestep::FixedTimestep(const IClock& clock, Duration tickDuration)
    : clock_(&clock), tickDuration_(tickDuration), previousTime_(clock.now()) {
  if (tickDuration <= Duration::zero()) {
    throw InvalidTickDurationException();
  }
}

std::size_t FixedTimestep::consumeTicks() {
  const Duration currentTime = clock_->now();
  accumulated_ += currentTime - previousTime_;
  previousTime_ = currentTime;

  const auto dueTicks = static_cast<std::size_t>(accumulated_ / tickDuration_);
  const std::size_t ticks = std::min(dueTicks, MAXIMUM_TICKS_PER_ADVANCE);
  const Duration consumed = tickDuration_ * static_cast<Duration::rep>(ticks);
  if (dueTicks > ticks) {
    droppedTime_ = accumulated_ - consumed;
    accumulated_ = Duration::zero();
  } else {
    droppedTime_ = Duration::zero();
    accumulated_ -= consumed;
  }
  return ticks;
}

Duration FixedTimestep::nextTickTime() const {
  return previousTime_ + tickDuration_ - accumulated_;
}

Duration FixedTimestep::droppedTime() const { return droppedTime_; }

}  // namespace rtype::engine
