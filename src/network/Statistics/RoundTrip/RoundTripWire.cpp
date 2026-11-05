#include "RoundTripWire.hpp"
#include <chrono>
#include <cstdint>
#include <optional>
#include "Heartbeat.hpp"
#include "RoundTripConstants.hpp"
#include "RoundTripFigures.hpp"
#include "RoundTrips.hpp"
#include "ServerInformation.hpp"
#include "ServerInformationRequest.hpp"

namespace rtype::network {

std::uint32_t wireMicrosecondsOf(std::chrono::nanoseconds time) {
  return static_cast<std::uint32_t>(
      std::chrono::duration_cast<std::chrono::microseconds>(time).count());
}

PlayerRoundTrip playerRoundTripOf(std::uint8_t playerNumber,
                                  const RoundTripFigures& figures) {
  return PlayerRoundTrip{.playerNumber = playerNumber,
                         .roundTrip = wireMicrosecondsOf(figures.roundTrip),
                         .jitter = wireMicrosecondsOf(figures.jitter)};
}

RoundTripFigures figuresOf(const PlayerRoundTrip& player) {
  return RoundTripFigures{
      .roundTrip = std::chrono::microseconds{player.roundTrip},
      .jitter = std::chrono::microseconds{player.jitter}};
}

std::optional<std::chrono::microseconds> echoedRoundTrip(
    const HeartbeatEcho& echo, std::chrono::nanoseconds receivedAt) {
  const std::uint32_t sinceSent = wireMicrosecondsOf(receivedAt) - echo.sentAt;
  if (echo.heldFor > sinceSent) {
    return std::nullopt;
  }
  const std::chrono::microseconds roundTrip{sinceSent - echo.heldFor};
  if (roundTrip > LONGEST_ROUND_TRIP) {
    return std::nullopt;
  }
  return roundTrip;
}

ServerInformationRequest informationRequestAt(std::chrono::nanoseconds now) {
  return ServerInformationRequest{.sentAt = wireMicrosecondsOf(now)};
}

std::optional<std::chrono::microseconds> informationRoundTripOf(
    const ServerInformation& answer, std::chrono::nanoseconds receivedAt) {
  return echoedRoundTrip(
      HeartbeatEcho{.sentAt = answer.sentAt, .heldFor = answer.heldFor},
      receivedAt);
}

}  // namespace rtype::network
