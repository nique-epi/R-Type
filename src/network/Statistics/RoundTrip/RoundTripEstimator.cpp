#include "RoundTripEstimator.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <optional>
#include "RoundTripConstants.hpp"
#include "RoundTripFigures.hpp"

namespace rtype::network {

void RoundTripEstimator::add(std::chrono::microseconds roundTrip) {
  smoothed_ = added_ == 0 ? roundTrip
                          : smoothed_ + (roundTrip - smoothed_) /
                                            ROUND_TRIP_SMOOTHING_DIVISOR;
  newest_.at(added_ % JITTER_WINDOW_SIZE) = roundTrip;
  ++added_;
}

std::optional<RoundTripFigures> RoundTripEstimator::figures() const {
  if (added_ == 0) {
    return std::nullopt;
  }
  return RoundTripFigures{.roundTrip = smoothed_, .jitter = jitter()};
}

std::chrono::microseconds RoundTripEstimator::jitter() const {
  const std::size_t kept = std::min(added_, JITTER_WINDOW_SIZE);
  if (kept < 2) {
    return std::chrono::microseconds::zero();
  }
  const std::size_t oldest = added_ - kept;
  std::chrono::microseconds gaps{0};
  for (std::size_t step = 1; step < kept; ++step) {
    const std::chrono::microseconds previous =
        newest_.at((oldest + step - 1) % JITTER_WINDOW_SIZE);
    const std::chrono::microseconds current =
        newest_.at((oldest + step) % JITTER_WINDOW_SIZE);
    gaps += std::chrono::abs(current - previous);
  }
  return gaps / static_cast<std::chrono::microseconds::rep>(kept - 1);
}

}  // namespace rtype::network
