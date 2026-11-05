#include "RoundTripWire.hpp"
#include <chrono>
#include <cstdint>
#include "RoundTripFigures.hpp"
#include "RoundTrips.hpp"

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

}  // namespace rtype::network
