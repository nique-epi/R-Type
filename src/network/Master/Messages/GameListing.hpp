#pragma once

#include <string>
#include <vector>
#include "ListedGame.hpp"

namespace rtype::network {

/** @brief The games a server hosts now, and the difficulties a new game of
 * it may be played at: the answer of `GET /api/servers/{id}/games`. */
struct GameListing {
  /** @brief MAXIMUM_HOSTED_GAMES at most. */
  std::vector<ListedGame> games;
  /** @brief The difficulties of its last heartbeat, MAXIMUM_DIFFICULTIES at
   * most; none while it sent none. */
  std::vector<std::string> difficulties;

  bool operator==(const GameListing&) const = default;
};

}  // namespace rtype::network
