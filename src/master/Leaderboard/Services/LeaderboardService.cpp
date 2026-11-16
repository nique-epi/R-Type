#include "LeaderboardService.hpp"
#include <charconv>
#include <chrono>
#include <cstdint>
#include <exception>
#include <optional>
#include <string>
#include <system_error>
#include <vector>
#include "FinishedGamePlayer.hpp"
#include "GameCredit.hpp"
#include "GameCreditOutcome.hpp"
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

/** @returns The account @p accountId names: its whole text a decimal number
 * from 1; nothing for a guest, or any other text. */
std::optional<std::int64_t> accountOf(
    const std::optional<std::string>& accountId) {
  if (!accountId.has_value() || accountId->empty()) {
    return std::nullopt;
  }
  std::int64_t userId = 0;
  const char* const end = accountId->data() + accountId->size();
  const auto [last, error] = std::from_chars(accountId->data(), end, userId);
  if (error != std::errc{} || last != end || userId < FIRST_ACCOUNT_ID) {
    return std::nullopt;
  }
  return userId;
}

/** @returns What @p player of @p result is paid at @p now, nothing when it
 * played as a guest. */
std::optional<GameCredit> creditOf(const ServerGameResult& result,
                                   const network::FinishedGamePlayer& player,
                                   std::chrono::sys_seconds now) {
  const std::optional<std::int64_t> userId = accountOf(player.accountId);
  if (!userId.has_value()) {
    return std::nullopt;
  }
  const std::uint32_t kopecks = network::kopecksFor(player.score);
  return GameCredit{
      .userId = *userId,
      .kopecks =
          result.game.victory ? network::withVictoryBonus(kopecks) : kopecks,
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
      if (wallet_->creditGame(*owed) == GameCreditOutcome::Credited) {
        logger_.info("credited game ", result.gameId, " to account ",
                     owed->userId, ": ", owed->kopecks, " kopecks, ",
                     owed->wonLooks.size(), " looks");
      }
    } catch (const std::exception& error) {
      logger_.error("game ", result.gameId, " was not credited to account ",
                    owed->userId, ": ", error.what());
    }
  }
}

}  // namespace rtype::master
