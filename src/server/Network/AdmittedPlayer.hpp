#pragma once

#include <cstdint>
#include "Endpoint.hpp"

namespace rtype::server {

/** @brief A player whose datagrams a DatagramSorter puts in the player queue.
 */
struct AdmittedPlayer {
  network::Endpoint endpoint;
  std::uint64_t sessionToken{};
};

}  // namespace rtype::server
