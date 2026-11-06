#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include "Endpoint.hpp"
#include "ServerCompatibility.hpp"
#include "ServerStatus.hpp"

namespace rtype::client {

/** @brief A server as the list of servers shows it: what the master lists of
 * it, and the ping the client measured. */
struct ServerRow {
  /** @brief The id the master gave it. */
  std::int64_t id{};
  std::string name;
  /** @brief The address and the UDP port players reach it on. */
  network::Endpoint endpoint;
  std::uint16_t playerCount{};
  std::uint16_t gameCount{};
  /** @brief The players its games hold at most, all its games together. */
  std::uint16_t maximumPlayers{};
  std::uint8_t maximumPlayersPerGame{};
  std::string gameVersion;
  /** @brief network::ServerStatus::Online or Degraded: the others are never
   * listed. */
  network::ServerStatus status{network::ServerStatus::Online};
  ServerCompatibility compatibility{ServerCompatibility::Compatible};
  /** @brief The round trip the client measured; nothing for a server of
   * another version, which never answers a request of this one. */
  std::optional<std::chrono::microseconds> ping;

  bool operator==(const ServerRow&) const = default;
};

}  // namespace rtype::client
