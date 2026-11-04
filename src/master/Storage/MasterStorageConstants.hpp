#pragma once

#include <array>
#include <string_view>

namespace rtype::master {

/**
 * @brief The first version of the database: the keys an admin issued, one per
 * game server. Only the fingerprint of a key is kept (see ServerKeys), so a
 * copy of the file lets nobody register as a server.
 */
constexpr std::string_view SERVER_KEYS_MIGRATION = R"sql(
CREATE TABLE server_keys (
  id          INTEGER PRIMARY KEY,
  name        TEXT    NOT NULL,
  fingerprint TEXT    NOT NULL UNIQUE,
  issued_at   INTEGER NOT NULL
) STRICT;
)sql";

/**
 * @brief The migrations of the database of the master, in order:
 * MASTER_MIGRATIONS[0] is version 1. A change adds a migration at the end and
 * never edits one a master already applied.
 */
constexpr std::array<std::string_view, 1> MASTER_MIGRATIONS{
    SERVER_KEYS_MIGRATION};

}  // namespace rtype::master
