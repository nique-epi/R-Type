#pragma once

#include <chrono>
#include "Endpoint.hpp"
#include "ServerInformation.hpp"

namespace rtype::client {

/** @brief What a game server answered a ServerInformationRequest with, and the
 * round trip it took. */
struct PingAnswer {
  /** @brief The address and the UDP port the request went to. */
  network::Endpoint server;
  /** @brief The time from the request to the answer, less the time the server
   * held it: the ping the list of servers shows. */
  std::chrono::microseconds roundTrip{};
  /** @brief What the server says of itself: its name, its versions, how full
   * its games are. */
  network::ServerInformation information;
};

}  // namespace rtype::client
