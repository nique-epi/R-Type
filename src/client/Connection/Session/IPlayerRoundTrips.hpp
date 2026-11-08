#pragma once

#include <cstdint>
#include <optional>
#include "RoundTripFigures.hpp"

namespace rtype::client {

/**
 * @brief The round trip between each player of the session and the server,
 * and its jitter, as a screen reads them.
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
};

}  // namespace rtype::client
