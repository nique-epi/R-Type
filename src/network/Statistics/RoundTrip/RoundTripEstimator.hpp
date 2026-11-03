#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <optional>
#include "RoundTripConstants.hpp"
#include "RoundTripFigures.hpp"

namespace rtype::network {

/**
 * @brief Turns round trips, given one at a time, into the figures a player
 * reads: the round trip smoothed as RFC 6298 smooths it, and the jitter, the
 * mean gap between two round trips in a row over the JITTER_WINDOW_SIZE
 * newest (the inter-packet delay variation of RFC 3393, on round trips). It
 * keeps those round trips in a fixed array: nothing is allocated.
 */
class RoundTripEstimator {
 public:
  /**
   * @brief Adds @p roundTrip. The first one becomes the smoothed round trip;
   * each next one moves it by its gap to it divided by
   * ROUND_TRIP_SMOOTHING_DIVISOR, rounded toward zero. Past
   * JITTER_WINDOW_SIZE round trips, the oldest one leaves the jitter.
   */
  void add(std::chrono::microseconds roundTrip);

  /** @returns The smoothed round trip and the jitter; nothing before the first
   * round trip. */
  [[nodiscard]] std::optional<RoundTripFigures> figures() const;

 private:
  /** @returns The mean gap between two round trips in a row among the ones
   * kept, oldest first; 0 with fewer than two. */
  [[nodiscard]] std::chrono::microseconds jitter() const;

  std::array<std::chrono::microseconds, JITTER_WINDOW_SIZE> newest_{};
  std::size_t count_{0};
  std::size_t next_{0};
  std::chrono::microseconds smoothed_{};
};

}  // namespace rtype::network
