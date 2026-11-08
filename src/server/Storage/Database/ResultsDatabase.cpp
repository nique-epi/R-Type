#include "ResultsDatabase.hpp"
#include <SQLiteCpp/Column.h>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <SQLiteCpp/Transaction.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include "DatabaseFile.hpp"
#include "GameResult.hpp"
#include "LeaderboardAnswer.hpp"
#include "LeaderboardRequest.hpp"
#include "PathText.hpp"
#include "ProtocolConstants.hpp"
#include "ResultsDatabaseConstants.hpp"
#include "ServerMigrationsConstants.hpp"
#include "UnsentGame.hpp"

namespace rtype::server {

namespace {

void bindGame(SQLite::Statement& insert, const GameResult& result,
              const std::string& gameId) {
  insert.bind(GAME_ID_PARAMETER, gameId);
  insert.bind(
      PLAYED_AT_PARAMETER,
      static_cast<std::int64_t>(result.endedAt.time_since_epoch().count()));
  insert.bind(DIFFICULTY_PARAMETER, result.difficulty);
  insert.bind(PLAYER_COUNT_PARAMETER, result.playerCount);
  insert.bind(VANILLA_PARAMETER, static_cast<std::int32_t>(result.vanilla));
}

void bindPlayer(SQLite::Statement& insert, const PlayerResult& player) {
  insert.bind(NICKNAME_PARAMETER, player.nickname);
  insert.bind(SCORE_PARAMETER, player.score);
  insert.bind(DEATHS_PARAMETER, player.deaths);
}

network::LeaderboardRow rowOf(const SQLite::Statement& query) {
  return network::LeaderboardRow{
      .rank = query.getColumn(BOARD_RANK_COLUMN).getUInt(),
      .score = query.getColumn(SCORE_COLUMN).getUInt(),
      .playedAt = query.getColumn(PLAYED_AT_COLUMN).getInt64(),
      .nickname = query.getColumn(NICKNAME_COLUMN).getString()};
}

/** @returns The game of the row @p query is on, without its players. */
UnsentGame unsentGameOf(const SQLite::Statement& query) {
  return UnsentGame{
      .gameId = query.getColumn(GAME_ID_COLUMN).getString(),
      .result = GameResult{
          .difficulty = query.getColumn(DIFFICULTY_COLUMN).getString(),
          .playerCount = static_cast<std::uint8_t>(
              query.getColumn(PLAYER_COUNT_COLUMN).getUInt()),
          .endedAt = std::chrono::sys_seconds{std::chrono::seconds{
              query.getColumn(PLAYED_AT_COLUMN).getInt64()}},
          .players = {},
          .vanilla = true}};
}

/** @returns The player of the row @p query is on. */
PlayerResult playerOf(const SQLite::Statement& query) {
  return PlayerResult{.nickname = query.getColumn(NICKNAME_COLUMN).getString(),
                      .score = query.getColumn(SCORE_COLUMN).getUInt(),
                      .deaths = query.getColumn(DEATHS_COLUMN).getUInt()};
}

/** @brief Writes @p answeredAt on every row of @p gameId with @p update. */
void markGame(SQLite::Statement& update, const std::string& gameId,
              std::chrono::sys_seconds answeredAt) {
  update.bind(GAME_ID_PARAMETER, gameId);
  update.bind(ANSWERED_AT_PARAMETER,
              static_cast<std::int64_t>(answeredAt.time_since_epoch().count()));
  update.exec();
}

}  // namespace

ResultsDatabase::ResultsDatabase(const std::filesystem::path& file)
    : database_(storage::openDatabaseFile(logging::genericText(file),
                                          SERVER_MIGRATIONS)) {}

std::string ResultsDatabase::save(const GameResult& result) {
  SQLite::Transaction transaction{database_,
                                  SQLite::TransactionBehavior::IMMEDIATE};
  SQLite::Statement insert{database_, std::string{INSERT_RESULT_STATEMENT}};
  const std::string gameId =
      database_.execAndGet(std::string{NEW_GAME_ID_QUERY}).getString();
  bindGame(insert, result, gameId);
  for (const PlayerResult& player : result.players) {
    bindPlayer(insert, player);
    insert.exec();
    insert.reset();
  }
  transaction.commit();
  return gameId;
}

network::LeaderboardAnswer ResultsDatabase::leaderboard(
    const network::LeaderboardRequest& request,
    const std::string& asker) const {
  SQLite::Statement query{database_, std::string{LEADERBOARD_QUERY}};
  query.bind(DIFFICULTY_PARAMETER, request.difficulty);
  query.bind(PLAYER_COUNT_PARAMETER, request.playerCount);
  query.bind(ASKER_PARAMETER, asker);
  query.bind(TOP_ROWS_PARAMETER,
             static_cast<std::int64_t>(network::MAXIMUM_LEADERBOARD_ROWS));
  network::LeaderboardAnswer answer{
      .asked = request, .rows = {}, .ownRow = std::nullopt};
  while (query.executeStep()) {
    const network::LeaderboardRow row = rowOf(query);
    if (answer.rows.size() < network::MAXIMUM_LEADERBOARD_ROWS) {
      answer.rows.push_back(row);
    }
    if (query.getColumn(IS_ASKER_COLUMN).getInt() != 0) {
      answer.ownRow = row;
    }
  }
  return answer;
}

std::vector<UnsentGame> ResultsDatabase::unsentVanillaGames(
    std::size_t gameLimit) const {
  SQLite::Statement query{database_, std::string{UNSENT_GAMES_QUERY}};
  query.bind(GAME_LIMIT_PARAMETER, static_cast<std::int64_t>(gameLimit));
  std::vector<UnsentGame> games;
  while (query.executeStep()) {
    if (games.empty() ||
        games.back().gameId != query.getColumn(GAME_ID_COLUMN).getString()) {
      games.push_back(unsentGameOf(query));
    }
    games.back().result.players.push_back(playerOf(query));
  }
  return games;
}

void ResultsDatabase::markSent(const std::string& gameId,
                               std::chrono::sys_seconds sentAt) {
  SQLite::Statement update{database_, std::string{MARK_SENT_STATEMENT}};
  markGame(update, gameId, sentAt);
}

void ResultsDatabase::markRefused(const std::string& gameId,
                                  std::chrono::sys_seconds refusedAt) {
  SQLite::Statement update{database_, std::string{MARK_REFUSED_STATEMENT}};
  markGame(update, gameId, refusedAt);
}

}  // namespace rtype::server
