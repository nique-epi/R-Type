#pragma once

#include "GameResultRepository.hpp"
#include "GlobalLeaderboard.hpp"
#include "LeaderboardQuery.hpp"
#include "MasterDatabase.hpp"
#include "ServerGameResult.hpp"

namespace rtype::master {

/** @brief The rules of the results the servers report and of the global
 * leaderboard read from them. */
class LeaderboardService {
 public:
  explicit LeaderboardService(MasterDatabase& database);

  /**
   * @brief Keeps a row per player of @p result; a row already kept stays as
   * it is.
   * @returns false, keeping nothing, when its game was not vanilla: the
   * leaderboard compares games played with the rules of the game only, and
   * the other games stay in the leaderboard of their server.
   */
  bool keepResult(const ServerGameResult& result);

  /** @returns The page of @p query, and the row of the player it names. */
  [[nodiscard]] network::GlobalLeaderboard leaderboardOf(
      const LeaderboardQuery& query);

 private:
  GameResultRepository results_;
};

}  // namespace rtype::master
