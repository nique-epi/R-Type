#pragma once

#include <cstdint>
#include "Endpoint.hpp"
#include "SenderAllowance.hpp"

namespace rtype::server {

/** @brief A player whose datagrams the DatagramSorter hands to the player
 * queue: where it sends from, the session token its datagrams carry, and what
 * the datagrams carrying that token spent of PLAYER_RATE_LIMIT. */
struct AdmittedPlayer {
  network::Endpoint endpoint;
  std::uint64_t sessionToken{};
  network::SenderAllowance allowance;
};

}  // namespace rtype::server
