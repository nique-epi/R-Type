#pragma once

#include <cstdint>
#include <optional>
#include "RoundTripFigures.hpp"
#include "RoundTripSample.hpp"

namespace rtype::client {

/**
 * @brief The round trip between each player of the session and the server,
 * and its jitter, as a screen reads them, and each round trip of the client
 * as it is measured.
 */
class IPlayerRoundTrips {
 public:
  virtual ~IPlayerRoundTrips() = default;

  /**
   * @returns The round trip and the jitter of the player numbered
   * @p playerNumber: what the client measured for its own player, what the
   * newest RoundTrips of the server lists for the others; nothing without a
   * session, nor before the first round trip of that player.
   */
  [[nodiscard]] virtual std::optional<network::RoundTripFigures> roundTripOf(
      std::uint8_t playerNumber) const = 0;

  /** @returns The newest round trip the client measured with its server, as
   * measured; nothing without a session, nor before its first round trip. */
  [[nodiscard]] virtual std::optional<RoundTripSample> newestOwnRoundTrip()
      const = 0;
};

}  // namespace rtype::client
