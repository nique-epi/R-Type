#pragma once

#include <chrono>

namespace rtype::network {

/** @brief What a RoundTripEstimator tells of the round trips it was given. */
struct RoundTripFigures {
  /** @brief The round trips smoothed: the first one, then each next one moves
   * it by 1/ROUND_TRIP_SMOOTHING_DIVISOR of its gap to it. */
  std::chrono::microseconds roundTrip{};
  /** @brief The mean gap between two round trips in a row, over the
   * JITTER_WINDOW_SIZE newest; 0 after a single one. */
  std::chrono::microseconds jitter{};

  bool operator==(const RoundTripFigures&) const = default;
};

}  // namespace rtype::network
