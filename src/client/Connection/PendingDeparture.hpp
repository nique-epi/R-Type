#pragma once

#include <cstdint>
#include "Endpoint.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief The copies of a Disconnection still due to a server the player
 * left, sent with the token and the sequences of the session left. */
struct PendingDeparture {
  network::Endpoint server;
  std::uint64_t sessionToken{};
  std::uint16_t nextSequence{};
  int copiesLeft{};
  engine::Duration sinceCopy{};
};

}  // namespace rtype::client
