#pragma once

#include <cstdint>
#include "Endpoint.hpp"

namespace rtype::server {

/** @brief A player whose datagrams the DatagramSorter hands to the player
 * queue: where it sends from, and the session token its datagrams carry. */
struct AdmittedPlayer {
  network::Endpoint endpoint;
  std::uint64_t sessionToken{};
};

}  // namespace rtype::server
