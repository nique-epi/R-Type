#include "ServerClock.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <optional>
#include <ranges>
#include "ClockEstimate.hpp"
#include "ClockSample.hpp"
#include "ServerClockConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

namespace {

/** @returns The estimate @p sample gives: the message arrived half of its
 * round trip after the server sent it, at tick sample.tick. */
ClockEstimate estimateOf(const ClockSample& sample) {
  const Duration sentAt =
      sample.receivedAt - sample.roundTrip / WAYS_OF_A_ROUND_TRIP;
  return ClockEstimate{
      .roundTrip = sample.roundTrip,
      .tickZero = sentAt - SIMULATION_TICK_DURATION * sample.tick};
}

}  // namespace

void ServerClock::add(const ClockSample& sample) {
  if (newestTick_.has_value() && sample.tick < *newestTick_) {
    reset();
  }
  newestTick_ = sample.tick;
  window_.push_back(estimateOf(sample));
  if (window_.size() > CLOCK_FILTER_WINDOW) {
    window_.pop_front();
  }
  follow(shortestRoundTrip());
}

void ServerClock::reset() {
  window_.clear();
  tickZero_.reset();
  newestTick_.reset();
}

std::size_t ServerClock::sampleCount() const { return window_.size(); }

std::optional<Duration> ServerClock::localInstantOf(std::uint32_t tick) const {
  if (!tickZero_.has_value()) {
    return std::nullopt;
  }
  return *tickZero_ + SIMULATION_TICK_DURATION * tick;
}

std::optional<double> ServerClock::tickAt(Duration localNow) const {
  if (!tickZero_.has_value()) {
    return std::nullopt;
  }
  return static_cast<double>((localNow - *tickZero_).count()) /
         static_cast<double>(SIMULATION_TICK_DURATION.count());
}

ClockEstimate ServerClock::shortestRoundTrip() const {
  return *std::ranges::min_element(window_ | std::views::reverse, {},
                                   &ClockEstimate::roundTrip);
}

void ServerClock::follow(const ClockEstimate& target) {
  if (!tickZero_.has_value()) {
    tickZero_ = target.tickZero;
    return;
  }
  const Duration gap = target.tickZero - *tickZero_;
  if (std::chrono::abs(gap) > CLOCK_SNAP_DISTANCE) {
    tickZero_ = target.tickZero;
    return;
  }
  *tickZero_ += gap / CLOCK_SMOOTHING_DIVISOR;
}

}  // namespace rtype::engine
