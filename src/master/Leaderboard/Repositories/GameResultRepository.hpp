#pragma once

#include "GlobalLeaderboard.hpp"
#include "LeaderboardQuery.hpp"
#include "MasterDatabase.hpp"
#include "ServerGameResult.hpp"

namespace rtype::master {

/** @brief The table `game_results`: the only code that writes its SQL. Each
 * call runs behind the lock of the MasterDatabase. */
class GameResultRepository {
 public:
  explicit GameResultRepository(MasterDatabase& database);

  /**
   * @brief Adds a row per player of @p result, in one transaction, unless the
   * master keeps a row of that game of that server already: a game is kept
   * whole and once, so a result sent again changes nothing, even with other
   * players or scores.
   */
  void add(const ServerGameResult& result);

  /** @returns The rows of the page of @p query, by rank, and the row of the
   * player it names, wherever it ranks. */
  [[nodiscard]] network::GlobalLeaderboard leaderboardOf(
      const LeaderboardQuery& query);

 private:
  MasterDatabase& database_;
};

}  // namespace rtype::master
