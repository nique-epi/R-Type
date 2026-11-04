#pragma once

#include <array>
#include <string_view>

namespace rtype::master {

/**
 * @brief The first version of the database: the game servers, and a sample of
 * each heartbeat at most once a second.
 *
 * A server is created with its name and the fingerprint of its key (see
 * fingerprintOf()), so a copy of the file lets nobody act as a server; its
 * registration columns stay null until it registers. The primary key of the
 * samples is the server and the second, and the table has no rowid, so a
 * sample is one row of one B-tree. The nicknames of the players live in the
 * last state only, never in a sample.
 */
constexpr std::string_view SERVERS_MIGRATION = R"sql(
CREATE TABLE servers (
  id                       INTEGER PRIMARY KEY,
  name                     TEXT    NOT NULL,
  key_fingerprint          TEXT    NOT NULL UNIQUE,
  created_by_address       TEXT    NOT NULL,
  created_at               INTEGER NOT NULL,
  address                  TEXT,
  udp_port                 INTEGER,
  game_version             TEXT,
  protocol_version         INTEGER,
  maximum_games            INTEGER,
  maximum_players_per_game INTEGER,
  registered_at            INTEGER,
  last_heartbeat           TEXT,
  last_heartbeat_at        INTEGER
) STRICT;

CREATE INDEX servers_by_creator ON servers (created_by_address, created_at);

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
) STRICT, WITHOUT ROWID;

CREATE INDEX heartbeat_samples_by_date ON heartbeat_samples (taken_at);
)sql";

/**
 * @brief The migrations of the database of the master, in order:
 * MASTER_MIGRATIONS[0] is version 1. A change adds a migration at the end and
 * never edits one a master already applied.
 */
constexpr std::array<std::string_view, 1> MASTER_MIGRATIONS{SERVERS_MIGRATION};

}  // namespace rtype::master
