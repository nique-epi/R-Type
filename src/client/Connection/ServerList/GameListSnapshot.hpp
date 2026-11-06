#pragma once

#include <vector>
#include "GameRow.hpp"
#include "ServerListPhase.hpp"

namespace rtype::client {

/** @brief The games of a server as a screen reads them at a frame. */
struct GameListSnapshot {
  ServerListPhase phase{ServerListPhase::Searching};
  /** @brief The games a player can join first, then by number; empty unless
   * phase is Ready. */
  std::vector<GameRow> games;

  bool operator==(const GameListSnapshot&) const = default;
};

}  // namespace rtype::client
