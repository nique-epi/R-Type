#pragma once

#include <vector>
#include "Endpoint.hpp"
#include "GameRequest.hpp"

namespace rtype::client {

/** @brief A game the server browser sends the player to: the server that
 * hosts it, and the game asked of it with its rules. */
struct ChosenGame {
  network::Endpoint server;
  GameRequest request;
  /** @brief The servers to try next, in order, when @ref server answers that
   * it is full; empty unless QUICK PLAY chose the game. */
  std::vector<network::Endpoint> otherServers;
};

}  // namespace rtype::client
