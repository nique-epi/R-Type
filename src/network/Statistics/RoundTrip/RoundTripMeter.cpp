#include "RoundTripMeter.hpp"
#include <chrono>
#include <optional>
#include "Heartbeat.hpp"
#include "PeerStamp.hpp"
#include "RoundTripConstants.hpp"
#include "RoundTripFigures.hpp"
#include "RoundTripWire.hpp"

namespace rtype::network {

Heartbeat RoundTripMeter::nextHeartbeat(std::chrono::nanoseconds now) const {
  Heartbeat heartbeat{.sentAt = wireMicrosecondsOf(now), .echo = std::nullopt};
  if (!newestReceived_.has_value()) {
    return heartbeat;
  }
  const std::chrono::nanoseconds held = now - newestReceived_->receivedAt;
  if (held <= LONGEST_ROUND_TRIP) {
    heartbeat.echo = HeartbeatEcho{.sentAt = newestReceived_->sentAt,
                                   .heldFor = wireMicrosecondsOf(held)};
  }
  return heartbeat;
}

std::optional<std::chrono::microseconds> RoundTripMeter::receive(
    const Heartbeat& heartbeat, std::chrono::nanoseconds receivedAt) {
  newestReceived_ =
      PeerStamp{.sentAt = heartbeat.sentAt, .receivedAt = receivedAt};
  if (!heartbeat.echo.has_value()) {
    return std::nullopt;
  }
  return measure(*heartbeat.echo, receivedAt);
}

std::optional<RoundTripFigures> RoundTripMeter::figures() const {
  return estimator_.figures();
}

std::optional<std::chrono::microseconds> RoundTripMeter::measure(
    const HeartbeatEcho& echo, std::chrono::nanoseconds receivedAt) {
  const std::optional<std::chrono::microseconds> roundTrip =
      echoedRoundTrip(echo, receivedAt);
  if (roundTrip.has_value()) {
    estimator_.add(*roundTrip);
  }
  return roundTrip;
}

}  // namespace rtype::network
