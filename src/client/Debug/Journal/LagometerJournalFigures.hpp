#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include "ClockRecalibrations.hpp"
#include "RoundTripFigures.hpp"
#include "SessionArrivals.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief What the lagometer writes in the journal for one
 * LAGOMETER_JOURNAL_PERIOD in which a game drew. */
struct LagometerJournalFigures {
  /** @brief How long the period lasted, from its first frame to its last. */
  engine::Duration period{};
  /** @brief The smoothed round trip and the jitter at its end. */
  std::optional<network::RoundTripFigures> roundTrip = std::nullopt;
  /** @brief The datagrams of the server accepted and missed in the period. */
  SessionArrivals arrivals;
  /** @brief The frames of the period, and those the game drew past the newest
   * state received. */
  std::uint64_t frames{0};
  std::uint64_t pastTheNewest{0};
  /** @brief The smallest and the largest delay behind the server a frame of
   * the game drew at. */
  std::chrono::microseconds smallestDelay{};
  std::chrono::microseconds largestDelay{};
  /** @brief The states of the period that moved the tick the other entities
   * are shown at, and how far in all, in server ticks. */
  ClockRecalibrations recalibrations;
  /** @brief The longest frame of the period, and the frames longer than
   * LAGOMETER_SLOW_FRAME. */
  engine::Duration longestFrame{};
  std::uint64_t slowFrames{0};

  bool operator==(const LagometerJournalFigures&) const = default;
};

}  // namespace rtype::client
