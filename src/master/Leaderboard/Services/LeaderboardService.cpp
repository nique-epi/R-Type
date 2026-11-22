#include "LeaderboardService.hpp"
#include <chrono>
#include <exception>
#include <optional>
#include <string>
#include <vector>
#include "FinishedGamePlayer.hpp"
#include "GameCredit.hpp"
#include "GameCreditOutcome.hpp"
#include "GameGain.hpp"
#include "GameKopecks.hpp"
#include "GlobalLeaderboard.hpp"
#include "IAccountWallet.hpp"
#include "LeaderboardQuery.hpp"
#include "LeaderboardServiceConstants.hpp"
#include "MasterConstants.hpp"
#include "MasterDatabase.hpp"
#include "ServerGameResult.hpp"

namespace rtype::master {

namespace {

/** @returns What @p player of @p result is paid at @p now, through the game
 * ticket it joined with; nothing when it played as a guest. The accountId a
 * server writes is never read: a server cannot know an account. */
std::optional<GameCredit> creditOf(const ServerGameResult& result,
                                   const network::FinishedGamePlayer& player,
                                   std::chrono::sys_seconds now) {
  if (!player.ticket.has_value()) {
    return std::nullopt;
  }
  return GameCredit{
      .ticket = *player.ticket,
      .kopecks = network::kopecksFor(
          network::GameGain{.score = player.score, .bounty = player.bounty}),
      .wonLooks = player.wonLooks,
      .reference = std::string{GAME_REFERENCE_PREFIX} +
                   std::to_string(result.serverId) +
                   std::string{GAME_REFERENCE_SEPARATOR} + result.gameId +
                   std::string{GAME_REFERENCE_SEPARATOR} + player.nickname,
      .creditedAt = now};
}

}  // namespace

LeaderboardService::LeaderboardService(MasterDatabase& database,
                                       IAccountWallet* wallet)
    : results_(database),
      wallet_(wallet),
      logger_(std::string{MASTER_LOGGER_NAME}) {}

bool LeaderboardService::keepResult(const ServerGameResult& result,
                                    std::chrono::sys_seconds now) {
  if (!result.game.vanilla) {
    return false;
  }
  if (results_.add(result) && wallet_ != nullptr) {
    credit(result, now);
  }
  return true;
}

network::GlobalLeaderboard LeaderboardService::leaderboardOf(
    const LeaderboardQuery& query) {
  return results_.leaderboardOf(query);
}

void LeaderboardService::credit(const ServerGameResult& result,
                                std::chrono::sys_seconds now) {
  for (const network::FinishedGamePlayer& player : result.game.players) {
    const std::optional<GameCredit> owed = creditOf(result, player, now);
    if (!owed.has_value()) {
      continue;
    }
    try {
      logCredit(*owed, wallet_->creditGame(*owed));
    } catch (const std::exception& error) {
      logger_.error(owed->reference, " was not credited: ", error.what());
    }
  }
}

void LeaderboardService::logCredit(const GameCredit& credit,
                                   GameCreditOutcome outcome) {
  switch (outcome) {
    case GameCreditOutcome::Credited:
      logger_.info(credit.reference,
                   " credited through its ticket: ", credit.kopecks,
                   " kopecks, ", credit.wonLooks.size(), " looks");
      return;
    case GameCreditOutcome::AlreadyCredited:
      logger_.info(credit.reference, " was credited before");
      return;
    case GameCreditOutcome::UnknownTicket:
      logger_.info(credit.reference,
                   " paid nothing: its ticket is unknown, used or expired");
      return;
    case GameCreditOutcome::UnknownAccount:
      logger_.info(credit.reference,
                   " paid nothing: the account of its ticket is gone");
      return;
  }
}

}  // namespace rtype::master
