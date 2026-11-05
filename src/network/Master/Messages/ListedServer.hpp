#pragma once

#include <cstdint>
#include "ServerRegistration.hpp"
#include "ServerStatus.hpp"

namespace rtype::network {

/** @brief A registered game server, as the master lists it to everyone. */
struct ListedServer {
  std::int64_t id{};
  /** @brief What it registered: its name, the address and the UDP port players
   * reach it on, its versions and its bounds; its address always known. */
  ServerRegistration registration;
  /** @brief The status the master computed last, every few seconds. */
  ServerStatus status{ServerStatus::Offline};
  /** @brief The games and players of its last heartbeat; 0 before one. */
  std::uint16_t gameCount{};
  std::uint16_t playerCount{};

  bool operator==(const ListedServer&) const = default;
};

}  // namespace rtype::network
