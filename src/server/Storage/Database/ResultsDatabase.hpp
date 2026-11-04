#pragma once

#include <SQLiteCpp/Database.h>
#include <filesystem>
#include <string>
#include "GameResult.hpp"
#include "LeaderboardAnswer.hpp"
#include "LeaderboardRequest.hpp"

namespace rtype::server {

/**
 * @brief The file of this server: a row per player and per finished game, and
 * what is read from them. The only place of the server that holds SQL.
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
   * @throws SQLite::Exception when SQLite refuses it; nothing is written then.
   */
  void save(const GameResult& result);

  /**
   * @returns The best MAXIMUM_LEADERBOARD_ROWS rows of the leaderboard of
   * @p request, and the row of @p asker when that player has one.
   * @throws SQLite::Exception when SQLite refuses it.
   */
  [[nodiscard]] network::LeaderboardAnswer leaderboard(
      const network::LeaderboardRequest& request,
      const std::string& asker) const;

 private:
  SQLite::Database database_;
};

}  // namespace rtype::server
