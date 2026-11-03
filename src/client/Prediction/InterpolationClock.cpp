#include "InterpolationClock.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <optional>
#include <span>
#include "InterpolationConstants.hpp"
#include "PositionHistoryConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns @p elapsed in server ticks, a fraction of a tick included. */
double ticksIn(engine::Duration elapsed) {
  return std::chrono::duration<double>(elapsed).count() *
         static_cast<double>(engine::SIMULATION_TICKS_PER_SECOND);
}

}  // namespace

void InterpolationClock::advance(engine::Duration elapsed) {
  if (!renderTick_.has_value()) {
    return;
  }
  const double latest =
      static_cast<double>(newestTick_) + MAXIMUM_EXTRAPOLATION_TICKS;
  renderTick_ = std::min(*renderTick_ + ticksIn(elapsed), latest);
}

void InterpolationClock::observe(std::uint32_t tick) {
  if (!renderTick_.has_value()) {
    newestTick_ = tick;
    renderTick_ = targetTick();
    return;
  }
  if (tick <= newestTick_) {
    return;
  }
  const double shown = *renderTick_;
  recordGap(tick - newestTick_);
  newestTick_ = tick;
  renderTick_ = pulledTowardsTarget(shown);
}

std::optional<double> InterpolationClock::renderTick() const {
  return renderTick_;
}

double InterpolationClock::targetTick() const {
  return static_cast<double>(newestTick_) -
         (INTERPOLATION_INTERVALS * intervalTicks());
}

double InterpolationClock::intervalTicks() const {
  if (gapCount_ == 0) {
    return ASSUMED_STATE_INTERVAL_TICKS;
  }
  const std::span<const std::uint32_t> kept{gaps_.data(), gapCount_};
  return static_cast<double>(*std::ranges::min_element(kept));
}

void InterpolationClock::recordGap(std::uint32_t gap) {
  if (gapCount_ == gaps_.size()) {
    std::ranges::rotate(gaps_, gaps_.begin() + 1);
    --gapCount_;
  }
  gaps_[gapCount_] = gap;
  ++gapCount_;
}

double InterpolationClock::pulledTowardsTarget(double shown) const {
  const double error = targetTick() - shown;
  const bool firstIntervalMeasured = gapCount_ == 1;
  if (firstIntervalMeasured ||
      std::abs(error) > SNAP_INTERVALS * intervalTicks()) {
    return targetTick();
  }
  return shown + (error * CORRECTION_GAIN);
}

}  // namespace rtype::client
