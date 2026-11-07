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
   * @brief Adds a row per player of @p result, in one transaction. A row of
   * the same server, game and nickname already kept stays as it is, so a
   * result sent again changes nothing.
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
