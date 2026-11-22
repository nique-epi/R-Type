#pragma once

#include <chrono>
#include "GameCredit.hpp"
#include "GameCreditOutcome.hpp"
#include "GameResultRepository.hpp"
#include "GlobalLeaderboard.hpp"
#include "IAccountWallet.hpp"
#include "LeaderboardQuery.hpp"
#include "Logger.hpp"
#include "MasterDatabase.hpp"
#include "ServerGameResult.hpp"

namespace rtype::master {

/** @brief The rules of the results the servers report, of the global
 * leaderboard read from them, and of what a result pays the accounts of its
 * players. */
class LeaderboardService {
 public:
  /** @param wallet Credits what a game pays; null when the master runs
   * without wallets, which pays nothing. It must outlive the service. */
  LeaderboardService(MasterDatabase& database, IAccountWallet* wallet);

  /**
   * @brief Keeps a row per player of @p result, unless its game is kept
   * already: then nothing changes. A result kept for the first time credits,
   * at @p now, the wallet of the account the master gave the ticket of each
   * player to, when that ticket was never used and has not expired:
   * network::kopecksFor() its score and its bounty, and the looks it won, once
   * per reference `game:<server>:<game>:<nickname>`. A credit that fails is
   * written in the journal and never refuses the result.
   * @returns false, keeping nothing, when its game was not vanilla: the
   * leaderboard compares games played with the rules of the game only, and
   * the other games stay in the leaderboard of their server.
   */
  bool keepResult(const ServerGameResult& result, std::chrono::sys_seconds now);

  /** @returns The page of @p query, and the row of the player it names. */
  [[nodiscard]] network::GlobalLeaderboard leaderboardOf(
      const LeaderboardQuery& query);

 private:
  /** @brief Credits each player of @p result who joined with a ticket. */
  void credit(const ServerGameResult& result, std::chrono::sys_seconds now);

  /** @brief Writes in the journal what crediting @p credit did, naming its
   * reference, never its ticket. */
  void logCredit(const GameCredit& credit, GameCreditOutcome outcome);

  GameResultRepository results_;
  IAccountWallet* wallet_;
  logging::Logger logger_;
};

}  // namespace rtype::master
