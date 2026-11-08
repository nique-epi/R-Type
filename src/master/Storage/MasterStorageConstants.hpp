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
 * @brief The second version: the status the master computed last for each
 * server, whether an admin put it in maintenance, every change of status, and
 * what the list and the statuses read of the last heartbeat (its games, its
 * players, its tick p95 and its loss), written with it so a read parses no
 * JSON. Those four are null before the first heartbeat and after a
 * registration. For a server a master of version 1 kept, they are read once
 * from its last heartbeat, so the list and the statuses see it as it was
 * before the upgrade; a text that is no JSON object with a list of games
 * leaves them null, and never stops the upgrade.
 *
 * A server starts offline: its first change comes with the first refresh after
 * its registration. The status is text with no CHECK, so a later status (the
 * unreachable one of the probe) needs no rebuild of the table; the master
 * writes only SERVER_STATUS_TEXTS. A change is keyed by its server and its
 * second, so a later change of the same second replaces it, and the table has
 * no rowid, as heartbeat_samples.
 */
constexpr std::string_view STATUSES_MIGRATION = R"sql(
ALTER TABLE servers ADD COLUMN status TEXT NOT NULL DEFAULT 'offline';
ALTER TABLE servers ADD COLUMN maintenance INTEGER NOT NULL DEFAULT 0;
ALTER TABLE servers ADD COLUMN last_game_count INTEGER;
ALTER TABLE servers ADD COLUMN last_player_count INTEGER;
ALTER TABLE servers ADD COLUMN last_tick_p95_milliseconds REAL;
ALTER TABLE servers ADD COLUMN last_loss_percent REAL;

UPDATE servers SET
  last_game_count = json_array_length(last_heartbeat, '$.games'),
  last_player_count = (
    SELECT COALESCE(SUM(json_array_length(game.value, '$.players')), 0)
    FROM json_each(servers.last_heartbeat, '$.games') AS game
    WHERE CASE WHEN game.type = 'object'
      THEN json_type(game.value, '$.players') = 'array' ELSE 0 END),
  last_tick_p95_milliseconds = CASE
    WHEN json_type(last_heartbeat, '$.measures.tickP95Milliseconds')
      IN ('integer', 'real')
    THEN json_extract(last_heartbeat, '$.measures.tickP95Milliseconds') END,
  last_loss_percent = CASE
    WHEN json_type(last_heartbeat, '$.measures.lossPercent')
      IN ('integer', 'real')
    THEN json_extract(last_heartbeat, '$.measures.lossPercent') END
WHERE CASE WHEN json_valid(last_heartbeat)
  THEN json_type(last_heartbeat, '$.games') = 'array' ELSE 0 END;

CREATE TABLE status_changes (
  server_id  INTEGER NOT NULL REFERENCES servers (id),
  changed_at INTEGER NOT NULL,
  status     TEXT    NOT NULL,
  PRIMARY KEY (server_id, changed_at)
) STRICT, WITHOUT ROWID;

CREATE INDEX status_changes_by_date ON status_changes (changed_at);
)sql";

/**
 * @brief The third version: the secret each server registered with, which the
 * probes of the master carry, and the second the master last received a valid
 * token of a probe of it. Both are null for a server registered before: its
 * next heartbeat is answered 409, and it registers again with its secret.
 */
constexpr std::string_view PROBES_MIGRATION = R"sql(
ALTER TABLE servers ADD COLUMN probe_secret INTEGER;
ALTER TABLE servers ADD COLUMN last_probe_answer_at INTEGER;
)sql";

/**
 * @brief The fourth version: a row per player of each vanilla game a server
 * reported, which the global leaderboard reads.
 *
 * A row keeps its server, so the leaderboard of one server reads its rows
 * alone, and the rows of a server can be erased with it. Its key is the
 * server, the id of the game the server drew and the nickname, compared with
 * NOCASE as a game server compares them (PlayerRoster::hasNickname()): a
 * result sent again adds no row. account_id is null for a guest. The table
 * has no rowid, as heartbeat_samples.
 *
 * The index serves a leaderboard, which reads one difficulty and one number
 * of players, and holds every column the ranking reads (server_id comes with
 * the primary key), so a read never goes back to the table: unlike the
 * single-column indexes of the other tables, it covers its query. The ranking
 * sorts by player before it sorts by score, so an index on the score alone was
 * slower than none.
 */
constexpr std::string_view RESULTS_MIGRATION = R"sql(
CREATE TABLE game_results (
  server_id    INTEGER NOT NULL REFERENCES servers (id),
  game_id      TEXT    NOT NULL,
  nickname     TEXT    NOT NULL COLLATE NOCASE,
  account_id   TEXT,
  score        INTEGER NOT NULL,
  played_at    INTEGER NOT NULL,
  difficulty   TEXT    NOT NULL,
  player_count INTEGER NOT NULL,
  PRIMARY KEY (server_id, game_id, nickname)
) STRICT, WITHOUT ROWID;

CREATE INDEX game_results_of_a_leaderboard
  ON game_results (difficulty, player_count, account_id, nickname, score,
                   played_at);
)sql";

/**
 * @brief The migrations of the database of the master, in order:
 * MASTER_MIGRATIONS[0] is version 1. A change adds a migration at the end and
 * never edits one a master already applied.
 */
constexpr std::array<std::string_view, 4> MASTER_MIGRATIONS{
    SERVERS_MIGRATION, STATUSES_MIGRATION, PROBES_MIGRATION, RESULTS_MIGRATION};

}  // namespace rtype::master
