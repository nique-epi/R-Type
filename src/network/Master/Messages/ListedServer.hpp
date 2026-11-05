#pragma once

#include <cstdint>
#include <string>
#include "ServerStatus.hpp"

namespace rtype::network {

/** @brief A registered game server, as the master lists it to everyone. */
struct ListedServer {
  std::int64_t id{};
  std::string name;
  /** @brief The IPv4 address and the UDP port players reach it on. */
  std::string address;
  std::uint16_t udpPort{};
  std::string gameVersion;
  std::uint8_t protocolVersion{};
  /** @brief The status the master computed last, every few seconds. */
  ServerStatus status{ServerStatus::Offline};
  /** @brief The games and players of its last heartbeat; 0 before one. */
  std::uint16_t gameCount{};
  std::uint16_t playerCount{};
  std::uint16_t maximumGames{};
  std::uint8_t maximumPlayersPerGame{};

  bool operator==(const ListedServer&) const = default;
};

}  // namespace rtype::network
