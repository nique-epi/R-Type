#pragma once

#include <string>
#include "Endpoint.hpp"

namespace rtype::client {

/**
 * @brief The server and the nickname a connection screen starts its fields
 * with: those given at launch, then those of the last attempt the player
 * started, so a lost game is joined again with one key.
 */
struct ServerChoice {
  network::Endpoint server;
  std::string nickname;
};

}  // namespace rtype::client
