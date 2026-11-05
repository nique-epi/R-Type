#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <optional>
#include "ClockEstimate.hpp"
#include "ClockSample.hpp"
#include "IServerClock.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

/**
 * @brief Follows the tick counter of the server on the clock of the client,
 * from the samples the server's heartbeats give.
 *
 * The two machines share no clock, so the only thing estimated is the instant,
 * on the clock of the client, of tick 0 of the server. A sample says the
 * server was at tick T when it sent the message, one way before the message
 * arrived; one way is taken as half of the round trip, as NTP does (RFC 5905):
 * a path whose two ways differ by some time moves the estimate by half of that
 * time, and nothing here can tell.
 *
 * The estimates of the CLOCK_FILTER_WINDOW newest samples are kept and the one
 * with the shortest round trip is followed, as the clock filter of NTP does:
 * a message that waited in a queue has a long round trip and is left aside.
 * The clock closes 1 / CLOCK_SMOOTHING_DIVISOR of the gap to that estimate at
 * each sample, so the instants it gives never jump, unless the gap is more
 * than CLOCK_SNAP_DISTANCE, or no estimate was held yet: it then takes the
 * estimate at once.
 *
 * A tick lower than the previous one means a new game: the samples of the
 * previous one are forgotten.
 *
 * Not thread-safe: use it from the frame loop.
 */
class ServerClock final : public IServerClock {
 public:
  /**
   * @brief Takes @p sample into account.
   * @param sample Its roundTrip must not be negative. A sample is given only
   * once the message that carries it passed the checks of its session, so a
   * forged one cannot move the clock.
   */
  void add(const ClockSample& sample);

  /** @brief Forgets every sample: both answers are empty again. */
  void reset();

  /** @returns How many samples the clock holds to choose from, at most
   * CLOCK_FILTER_WINDOW. */
  [[nodiscard]] std::size_t sampleCount() const;

  [[nodiscard]] std::optional<Duration> localInstantOf(
      std::uint32_t tick) const override;
  [[nodiscard]] std::optional<double> tickAt(Duration localNow) const override;

 private:
  /** @returns The estimate with the shortest round trip; the newest of those
   * that tie. The window must not be empty. */
  [[nodiscard]] ClockEstimate shortestRoundTrip() const;

  /** @brief Moves the clock toward @p target, or onto it (see the class). */
  void follow(const ClockEstimate& target);

  std::deque<ClockEstimate> window_;
  std::optional<Duration> tickZero_;
  std::optional<std::uint32_t> newestTick_;
};

}  // namespace rtype::engine
