#include "RoundTripMeter.hpp"
#include <chrono>
#include <cstdint>
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
  const std::uint32_t sinceSent = wireMicrosecondsOf(receivedAt) - echo.sentAt;
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
