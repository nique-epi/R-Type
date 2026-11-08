#pragma once

#include <SQLiteCpp/Database.h>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>
#include "GameResult.hpp"
#include "LeaderboardAnswer.hpp"
#include "LeaderboardRequest.hpp"
#include "UnsentGame.hpp"

namespace rtype::server {

/**
 * @brief The file of this server: a row per player and per finished game, and
 * what is read from them. With BansDatabase, the only place of the server
 * that holds SQL.
 *
 * A leaderboard reads one difficulty and one number of players, so two
 * difficulties never mix. Every value reaches SQLite as a bound parameter, so a
 * nickname is never read as SQL.
 *
 * Blocks on the disk: use it from the storage thread only (see ResultsArchive).
 */
class ResultsDatabase {
 public:
  /**
   * @param file The database of this server, created when it does not exist.
   * @throws storage::StorageException when it cannot be opened (see
   * storage::openDatabaseFile()).
   */
  explicit ResultsDatabase(const std::filesystem::path& file);

  /**
   * @brief Writes one row per player of @p result, under a new game identifier,
   * in one transaction: after a crash, either every row is there or none is.
   * @returns The identifier drawn for the game.
   * @throws SQLite::Exception when SQLite refuses it; nothing is written then.
   */
  std::string save(const GameResult& result);

  /**
   * @returns The best MAXIMUM_LEADERBOARD_ROWS rows of the leaderboard of
   * @p request, and the row of @p asker when that player has one.
   * @throws SQLite::Exception when SQLite refuses it.
   */
  [[nodiscard]] network::LeaderboardAnswer leaderboard(
      const network::LeaderboardRequest& request,
      const std::string& asker) const;

  /**
   * @returns The @p gameLimit oldest vanilla games neither sent to the master
   * nor refused by it, the oldest first, each with all its rows. A guest is
   * the only kind of player read: no game writes account_id yet.
   * @throws SQLite::Exception when SQLite refuses it.
   */
  [[nodiscard]] std::vector<UnsentGame> unsentVanillaGames(
      std::size_t gameLimit) const;

  /**
   * @brief Writes @p sentAt as the second the master accepted the game
   * @p gameId, in sent_at of each of its rows, in one statement, which SQLite
   * runs as one transaction: the game is never read as unsent again.
   * @throws SQLite::Exception when SQLite refuses it; nothing is written then.
   */
  void markSent(const std::string& gameId, std::chrono::sys_seconds sentAt);

  /**
   * @brief Writes @p refusedAt as the second the master refused the game
   * @p gameId for good, in refused_at, as markSent() does: the game stays in
   * the leaderboard of this server and is never sent again.
   * @throws SQLite::Exception when SQLite refuses it; nothing is written then.
   */
  void markRefused(const std::string& gameId,
                   std::chrono::sys_seconds refusedAt);

 private:
  SQLite::Database database_;
};

}  // namespace rtype::server
