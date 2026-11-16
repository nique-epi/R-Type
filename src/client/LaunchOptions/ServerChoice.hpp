#pragma once

#include <optional>
#include <string>
#include <vector>
#include "ConnectionRequest.hpp"
#include "Endpoint.hpp"
#include "GameRequest.hpp"

namespace rtype::client {

/**
 * @brief The server, the nickname and the game a connection screen starts
 * with: those given at launch, then those of the last attempt the player
 * started, so a lost game is joined again with one key. The form has no field
 * for the game: it asks for the game given at launch, while a screen the
 * server map opens asks for the game the map chose.
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
  /** @brief The game of the last attempt, whichever screen started it, with
   * its rules when known; nothing before the first. A screen that starts with
   * lastServer asks for it, and the waiting room shows its rules. */
  std::optional<GameRequest> lastRequest = std::nullopt;
  /** @brief The servers to try, in order, once lastServer answers that it is
   * full: those QUICK PLAY of the server map kept behind the one it chose.
   * Empty for any other choice of the map, which writes it each time, and
   * read only by the screen the map opens. */
  std::vector<network::Endpoint> nextServers;
};

}  // namespace rtype::client
