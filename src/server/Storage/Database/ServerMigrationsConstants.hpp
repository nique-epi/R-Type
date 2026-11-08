#pragma once

#include <array>
#include <string_view>
#include "ResultsDatabaseConstants.hpp"

namespace rtype::server {

/**
 * @brief The third version of the file of a server: the table of the bans the
 * host typed in the console. The console no longer bans anyone, and the fourth
 * version drops the table; this one stays, so a file that already holds it
 * still opens, and a new file goes through both.
 */
constexpr std::string_view BANS_TABLE_MIGRATION = R"sql(
CREATE TABLE bans (
  kind       TEXT    NOT NULL CHECK (kind IN ('nickname', 'address')),
  target     TEXT    NOT NULL COLLATE NOCASE,
  banned_at  INTEGER NOT NULL,
  expires_at INTEGER,
  PRIMARY KEY (kind, target)
) STRICT;
)sql";

/** @brief The fourth version of the file of a server: drops the table of the
 * bans, which nothing reads any more. */
constexpr std::string_view DROP_BANS_TABLE_MIGRATION = "DROP TABLE bans;";

/** @brief The versions of the file of a server, in order, for every process
 * that opens it, the supervisor and each game: a change adds one, never edits
 * one, and a process with a shorter list refuses the file (see
 * storage::openDatabaseFile()). */
constexpr std::array<std::string_view, 4> SERVER_MIGRATIONS{
    RESULTS_TABLE_MIGRATION, UNSENT_GAMES_MIGRATION, BANS_TABLE_MIGRATION,
    DROP_BANS_TABLE_MIGRATION};

}  // namespace rtype::server
