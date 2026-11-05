#pragma once

#include <vector>
#include "ListedGame.hpp"

namespace rtype::network {

/** @brief The games a server hosts now: the answer of
 * `GET /api/servers/{id}/games`. */
struct GameListing {
  /** @brief MAXIMUM_HOSTED_GAMES at most. */
  std::vector<ListedGame> games;

  bool operator==(const GameListing&) const = default;
};

}  // namespace rtype::network
