#pragma once

#include <optional>
#include <vector>
#include "GlobalLeaderboardRow.hpp"

namespace rtype::network {

/** @brief One page of a leaderboard of the master, and the row of the player
 * it was asked for: the answer of `GET /api/leaderboard`. */
struct GlobalLeaderboard {
  /** @brief By rank, MAXIMUM_PAGE_SIZE at most. */
  std::vector<GlobalLeaderboardRow> scores;
  /** @brief The row of the player the request named, wherever it ranks; none
   * when it named none, or when that player has no score there. */
  std::optional<GlobalLeaderboardRow> player = std::nullopt;

  bool operator==(const GlobalLeaderboard&) const = default;
};

}  // namespace rtype::network
