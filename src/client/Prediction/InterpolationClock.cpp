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

void InterpolationClock::follow(std::optional<double> serverTick) {
  serverTick_ = serverTick;
}

void InterpolationClock::observe(std::uint32_t tick) {
  if (!renderTick_.has_value()) {
    newestTick_ = tick;
    recordLateness(tick);
    renderTick_ = targetTick();
    return;
  }
  if (tick <= newestTick_) {
    return;
  }
  const double shown = *renderTick_;
  gaps_.push(tick - newestTick_);
  newestTick_ = tick;
  recordLateness(tick);
  renderTick_ = pulledTowardsTarget(shown);
}

std::optional<double> InterpolationClock::renderTick() const {
  return renderTick_;
}

double InterpolationClock::referenceTick() const {
  const std::span<const double> kept = lateness_.values();
  if (!serverTick_.has_value() || kept.empty()) {
    return static_cast<double>(newestTick_);
  }
  return *serverTick_ - *std::ranges::max_element(kept);
}

double InterpolationClock::targetTick() const {
  return referenceTick() - (INTERPOLATION_INTERVALS * intervalTicks());
}

void InterpolationClock::recordLateness(std::uint32_t tick) {
  if (!serverTick_.has_value()) {
    return;
  }
  const double lateness = *serverTick_ - static_cast<double>(tick);
  if (lateness < EARLIEST_PLAUSIBLE_LATENESS_TICKS ||
      lateness > LONGEST_PLAUSIBLE_LATENESS_TICKS) {
    return;
  }
  lateness_.push(std::max(lateness, 0.0));
}

double InterpolationClock::intervalTicks() const {
  const std::span<const std::uint32_t> kept = gaps_.values();
  if (kept.empty()) {
    return ASSUMED_STATE_INTERVAL_TICKS;
  }
  return static_cast<double>(*std::ranges::min_element(kept));
}

double InterpolationClock::pulledTowardsTarget(double shown) const {
  const double error = targetTick() - shown;
  const bool firstIntervalMeasured = gaps_.values().size() == 1;
  if (firstIntervalMeasured ||
      std::abs(error) > SNAP_INTERVALS * intervalTicks()) {
    return targetTick();
  }
  return shown + (error * CORRECTION_GAIN);
}

}  // namespace rtype::client
