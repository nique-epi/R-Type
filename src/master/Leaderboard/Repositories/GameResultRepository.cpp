#include "GameResultRepository.hpp"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Statement.h>
#include <cstdint>
#include <string>
#include "EpochSeconds.hpp"
#include "FinishedGamePlayer.hpp"
#include "GlobalLeaderboard.hpp"
#include "GlobalLeaderboardRow.hpp"
#include "LeaderboardQuery.hpp"
#include "LeaderboardRepositoryConstants.hpp"
#include "MasterDatabase.hpp"
#include "ServerGameResult.hpp"
#include "ServersRepositoryConstants.hpp"
#include "StatementParts.hpp"

namespace rtype::master {

namespace {

/** @returns Whether @p connection keeps a row of the game of @p result. */
bool isKept(SQLite::Database& connection, const ServerGameResult& result) {
  SQLite::Statement query{connection, std::string{GAME_KEPT_QUERY}};
  query.bind(std::string{SERVER_ID_PARAMETER}, result.serverId);
  query.bind(std::string{GAME_ID_PARAMETER}, result.gameId);
  return query.executeStep();
}

void insertRow(SQLite::Database& connection, const ServerGameResult& result,
               const network::FinishedGamePlayer& player) {
  SQLite::Statement insert{connection, std::string{INSERT_RESULT_STATEMENT}};
  insert.bind(std::string{SERVER_ID_PARAMETER}, result.serverId);
  insert.bind(std::string{GAME_ID_PARAMETER}, result.gameId);
  insert.bind(std::string{NICKNAME_PARAMETER}, player.nickname);
  if (player.accountId.has_value()) {
    insert.bind(std::string{ACCOUNT_ID_PARAMETER}, *player.accountId);
  }
  insert.bind(std::string{SCORE_PARAMETER},
              static_cast<std::int64_t>(player.score));
  insert.bind(std::string{PLAYED_AT_PARAMETER},
              epochSecondsOf(result.game.playedAt));
  insert.bind(std::string{DIFFICULTY_PARAMETER}, result.game.difficulty);
  insert.bind(std::string{PLAYER_COUNT_PARAMETER},
              static_cast<int>(result.game.playerCount));
  insert.exec();
}

/** @brief Binds the filters and the page of @p query; a filter it leaves out
 * stays null. */
void bindQuery(SQLite::Statement& select, const LeaderboardQuery& query) {
  select.bind(std::string{DIFFICULTY_PARAMETER}, query.difficulty);
  select.bind(std::string{PLAYER_COUNT_PARAMETER},
              static_cast<int>(query.playerCount));
  if (query.serverId.has_value()) {
    select.bind(std::string{SERVER_ID_PARAMETER}, *query.serverId);
  }
  if (query.player.has_value()) {
    select.bind(std::string{PLAYER_PARAMETER}, *query.player);
  }
  bindPage(select, query.page);
}

network::GlobalLeaderboardRow rowIn(const SQLite::Statement& select) {
  return network::GlobalLeaderboardRow{
      .rank = select.getColumn(BOARD_RANK_COLUMN).getInt64(),
      .nickname = select.getColumn(NICKNAME_COLUMN).getString(),
      .score =
          static_cast<std::uint32_t>(select.getColumn(SCORE_COLUMN).getInt64()),
      .playedAt =
          dateOfEpochSeconds(select.getColumn(PLAYED_AT_COLUMN).getInt64()),
      .serverId = select.getColumn(BOARD_SERVER_ID_COLUMN).getInt64(),
      .serverName = select.getColumn(SERVER_NAME_COLUMN).getString()};
}

/** @brief Files the row @p select is on in @p leaderboard: in its page, as
 * its player, or both; the first row of the player is its best. */
void fileRow(const SQLite::Statement& select,
             network::GlobalLeaderboard& leaderboard) {
  const network::GlobalLeaderboardRow row = rowIn(select);
  if (select.getColumn(IN_PAGE_COLUMN).getInt() != 0) {
    leaderboard.scores.push_back(row);
  }
  if (select.getColumn(NAMED_COLUMN).getInt() != 0 &&
      !leaderboard.player.has_value()) {
    leaderboard.player = row;
  }
}

}  // namespace

GameResultRepository::GameResultRepository(MasterDatabase& database)
    : database_(database) {}

void GameResultRepository::add(const ServerGameResult& result) {
  database_.transact([&result](SQLite::Database& connection) {
    if (isKept(connection, result)) {
      return;
    }
    for (const network::FinishedGamePlayer& player : result.game.players) {
      insertRow(connection, result, player);
    }
  });
}

network::GlobalLeaderboard GameResultRepository::leaderboardOf(
    const LeaderboardQuery& query) {
  return database_.run([&query](SQLite::Database& connection) {
    SQLite::Statement select{connection, std::string{LEADERBOARD_QUERY}};
    bindQuery(select, query);
    network::GlobalLeaderboard leaderboard;
    while (select.executeStep()) {
      fileRow(select, leaderboard);
    }
    return leaderboard;
  });
}

}  // namespace rtype::master
