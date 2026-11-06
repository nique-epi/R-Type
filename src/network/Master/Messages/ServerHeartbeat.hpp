#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "HostedGame.hpp"
#include "ServerMeasures.hpp"

namespace rtype::network {

/**
 * @brief What a server tells the master every few seconds, and at once when a
 * player comes or goes: its whole state, which replaces the one before.
 */
struct ServerHeartbeat {
  /** @brief The games it hosts, MAXIMUM_HOSTED_GAMES at most. */
  std::vector<HostedGame> games;
  ServerMeasures measures;
  /** @brief The token of the last probe of the master, once; none before the
   * first probe, or when no probe came since the last heartbeat. */
  std::optional<std::uint64_t> probeToken = std::nullopt;

  bool operator==(const ServerHeartbeat&) const = default;
};

}  // namespace rtype::network
