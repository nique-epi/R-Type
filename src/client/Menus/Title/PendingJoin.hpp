#pragma once

#include <string>
#include "Endpoint.hpp"
#include "GameRequest.hpp"

namespace rtype::client {

/** @brief An attempt the connection screen starts once it has the game ticket
 * of the player, or knows it joins as a guest: the server, the nickname and
 * the game it asks for. */
struct PendingJoin {
  network::Endpoint server;
  std::string nickname;
  GameRequest request;
};

}  // namespace rtype::client
