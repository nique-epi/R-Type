#pragma once

#include <cstdint>
#include <string>
#include "FinishedGame.hpp"

namespace rtype::master {

/** @brief A finished game, with the server that reported it and the id that
 * server drew for it. */
struct ServerGameResult {
  std::int64_t serverId{};
  /** @brief Well formed (network::isWellFormedGameId()). */
  std::string gameId;
  network::FinishedGame game;
};

}  // namespace rtype::master
