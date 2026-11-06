#pragma once

#include <string>
#include "ConnectionRequest.hpp"
#include "Endpoint.hpp"

namespace rtype::client {

/**
 * @brief The server, the nickname and the game a connection screen starts
 * with: those given at launch, then those of the last attempt the player
 * started, so a lost game is joined again with one key. The screen has no
 * field for the game: every attempt asks for the game given at launch.
 */
struct ServerChoice {
  network::Endpoint server;
  std::string nickname;
  network::GameChoice game{network::ANY_GAME_CHOICE};
};

}  // namespace rtype::client
