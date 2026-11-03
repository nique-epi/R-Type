#include "RoundTripMeter.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include "Heartbeat.hpp"
#include "PeerStamp.hpp"
#include "RoundTripConstants.hpp"
#include "RoundTripFigures.hpp"

namespace rtype::network {

namespace {

/** @returns @p time in whole microseconds, modulo 2^32, as a Heartbeat
 * carries it. */
std::uint32_t wireMicrosecondsOf(std::chrono::nanoseconds time) {
  return static_cast<std::uint32_t>(
      std::chrono::duration_cast<std::chrono::microseconds>(time).count());
}

}  // namespace

Heartbeat RoundTripMeter::nextHeartbeat(std::chrono::nanoseconds now) const {
  Heartbeat heartbeat{.sentAt = wireMicrosecondsOf(now), .echo = std::nullopt};
  if (newestReceived_.has_value() &&
      now - newestReceived_->receivedAt <= LONGEST_ROUND_TRIP) {
    heartbeat.echo = HeartbeatEcho{
        .sentAt = newestReceived_->sentAt,
        .heldFor = wireMicrosecondsOf(now - newestReceived_->receivedAt)};
  }
  return heartbeat;
}

void RoundTripMeter::receive(const Heartbeat& heartbeat,
                             std::chrono::nanoseconds receivedAt) {
  newestReceived_ =
      PeerStamp{.sentAt = heartbeat.sentAt, .receivedAt = receivedAt};
  if (heartbeat.echo.has_value()) {
    measure(*heartbeat.echo, receivedAt);
  }
}

std::optional<RoundTripFigures> RoundTripMeter::figures() const {
  return estimator_.figures();
}

void RoundTripMeter::measure(const HeartbeatEcho& echo,
                             std::chrono::nanoseconds receivedAt) {
  const auto sinceSent =
      static_cast<std::uint32_t>(wireMicrosecondsOf(receivedAt) - echo.sentAt);
  if (echo.heldFor > sinceSent) {
    return;
  }
  const std::chrono::microseconds roundTrip{sinceSent - echo.heldFor};
  if (roundTrip > LONGEST_ROUND_TRIP) {
    return;
  }
  estimator_.add(roundTrip);
}

}  // namespace rtype::network
