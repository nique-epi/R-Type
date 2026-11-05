#pragma once

#include <chrono>
#include <optional>
#include "Heartbeat.hpp"
#include "PeerStamp.hpp"
#include "RoundTripEstimator.hpp"
#include "RoundTripFigures.hpp"

namespace rtype::network {

/**
 * @brief Measures the round trip of a session from the Heartbeats both sides
 * send each other, as RTCP does with its reports (RFC 3550, sections 6.4.1 and
 * A.8): each Heartbeat carries the time it is sent, on the clock of its
 * sender, and echoes the newest Heartbeat its sender received, with how long
 * it held it. When an echo comes back, the time since the echoed Heartbeat was
 * sent, less the time the peer held it, is one round trip; both times are
 * read on the clock of this side, so the two clocks never need to agree. The
 * time the peer held it is left out as RFC 9002 leaves out ack_delay
 * (section 5.3).
 *
 * Every time it is given is the time since the epoch of one steady clock: the
 * time a datagram arrived (IncomingDatagram::receivedAt) and the time read
 * when a Heartbeat is built. An echo that gives no round trip from 0 to
 * LONGEST_ROUND_TRIP is ignored: one held longer than since it was sent, or
 * one whose round trip is longer than LONGEST_ROUND_TRIP. The meter does not
 * remember the sentAt it sent, so an echo of a sentAt it never sent is
 * measured all the same when it falls in that range: a peer can fix its own
 * round trip anywhere from 0 to LONGEST_ROUND_TRIP.
 *
 * Not thread-safe.
 */
class RoundTripMeter {
 public:
  /**
   * @returns The Heartbeat to send at @p now: sentAt from @p now, and the echo
   * of the newest Heartbeat received, held from its arrival to @p now; no
   * echo before the first one, nor once it arrived more than
   * LONGEST_ROUND_TRIP ago.
   */
  [[nodiscard]] Heartbeat nextHeartbeat(std::chrono::nanoseconds now) const;

  /**
   * @brief Keeps @p heartbeat to echo it, and measures a round trip from its
   * echo, if it carries one.
   * @param receivedAt When the datagram of @p heartbeat arrived. Give the
   * Heartbeats in the order their datagrams were accepted: a session drops a
   * datagram that is not newer than the newest one it accepted, so the one
   * kept is the newest.
   * @returns The round trip this Heartbeat gave, before it is smoothed: the
   * one of the exchange @p heartbeat closes, which includes the trip of
   * @p heartbeat itself. Nothing when it carries no echo or its echo is
   * ignored (see the class).
   */
  std::optional<std::chrono::microseconds> receive(
      const Heartbeat& heartbeat, std::chrono::nanoseconds receivedAt);

  /** @returns The figures of the round trips measured; nothing before the
   * first one. */
  [[nodiscard]] std::optional<RoundTripFigures> figures() const;

 private:
  std::optional<std::chrono::microseconds> measure(
      const HeartbeatEcho& echo, std::chrono::nanoseconds receivedAt);

  std::optional<PeerStamp> newestReceived_;
  RoundTripEstimator estimator_;
};

}  // namespace rtype::network
