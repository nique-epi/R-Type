#pragma once

#include <vector>
#include "AccountLeaderboardRow.hpp"
#include "GlobalLeaderboard.hpp"
#include "LeaderboardQuery.hpp"
#include "MasterDatabase.hpp"
#include "PageRequest.hpp"
#include "ServerGameResult.hpp"

namespace rtype::master {

/** @brief The table `game_results`: the only code that writes its SQL. Each
 * call runs behind the lock of the MasterDatabase. The leaderboard of the
 * accounts also reads `credited_results` and `users`, which it never
 * writes. */
class GameResultRepository {
 public:
  explicit GameResultRepository(MasterDatabase& database);

  /**
   * @brief Adds a row per player of @p result, in one transaction, unless the
   * master keeps a row of that game of that server already: a game is kept
   * whole and once, so a result sent again changes nothing, even with other
   * players or scores.
   * @returns Whether it added the rows: false for a game kept already.
   */
  bool add(const ServerGameResult& result);

  /** @returns The rows of the page of @p query, by rank, and the row of the
   * player it names, wherever it ranks. */
  [[nodiscard]] network::GlobalLeaderboard leaderboardOf(
      const LeaderboardQuery& query);

  /** @returns The accounts of @p page, by rank: the total of the scores of
   * the games credited to each (ACCOUNT_LEADERBOARD_QUERY). */
  [[nodiscard]] std::vector<network::AccountLeaderboardRow>
  accountLeaderboardOf(const PageRequest& page);

 private:
  MasterDatabase& database_;
};

}  // namespace rtype::master
