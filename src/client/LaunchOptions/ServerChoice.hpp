#pragma once

#include <optional>
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
  /** @brief The server the form opened by ADDRESS or JOIN A SERVER starts
   * with: the one given at launch or by client.cfg, then the one of the last
   * attempt started from that form. The server map never changes it. */
  network::Endpoint server;
  std::string nickname;
  network::GameChoice game{network::ANY_GAME_CHOICE};
  /** @brief The server of the last attempt, whichever screen started it;
   * nothing before the first. A screen that connects at once starts with
   * it, and so does the form RETRY opens after a lost game. */
  std::optional<network::Endpoint> lastServer = std::nullopt;
};

}  // namespace rtype::client
