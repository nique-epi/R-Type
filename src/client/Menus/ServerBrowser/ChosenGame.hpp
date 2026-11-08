#pragma once

#include "Endpoint.hpp"
#include "GameRequest.hpp"

namespace rtype::client {

/** @brief A game the server browser sends the player to: the server that
 * hosts it, and the game asked of it with its rules. */
struct ChosenGame {
  network::Endpoint server;
  GameRequest request;
};

}  // namespace rtype::client
