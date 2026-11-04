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
 * @brief The second version: the game servers, one per key, with the last
 * state their heartbeat told, and a sample of each heartbeat at most once a
 * second (the primary key of the samples is the server and the second). The
 * nicknames of the players live in the last state only, never in a sample.
 */
constexpr std::string_view SERVERS_MIGRATION = R"sql(
CREATE TABLE servers (
  id                       INTEGER PRIMARY KEY,
  key_id                   INTEGER NOT NULL UNIQUE REFERENCES server_keys (id),
  name                     TEXT    NOT NULL,
  address                  TEXT    NOT NULL,
  udp_port                 INTEGER NOT NULL,
  game_version             TEXT    NOT NULL,
  protocol_version         INTEGER NOT NULL,
  maximum_games            INTEGER NOT NULL,
  maximum_players_per_game INTEGER NOT NULL,
  registered_at            INTEGER NOT NULL,
  last_heartbeat           TEXT,
  last_heartbeat_at        INTEGER
) STRICT;

CREATE TABLE heartbeat_samples (
  server_id                 INTEGER NOT NULL REFERENCES servers (id),
  taken_at                  INTEGER NOT NULL,
  game_count                INTEGER NOT NULL,
  player_count              INTEGER NOT NULL,
  tick_p50_milliseconds     REAL,
  tick_p95_milliseconds     REAL,
  received_bytes_per_second REAL,
  sent_bytes_per_second     REAL,
  loss_percent              REAL,
  average_ping_milliseconds REAL,
  PRIMARY KEY (server_id, taken_at)
) STRICT;

CREATE INDEX heartbeat_samples_by_date ON heartbeat_samples (taken_at);
)sql";

/**
 * @brief The migrations of the database of the master, in order:
 * MASTER_MIGRATIONS[0] is version 1. A change adds a migration at the end and
 * never edits one a master already applied.
 */
constexpr std::array<std::string_view, 2> MASTER_MIGRATIONS{
    SERVER_KEYS_MIGRATION, SERVERS_MIGRATION};

}  // namespace rtype::master
