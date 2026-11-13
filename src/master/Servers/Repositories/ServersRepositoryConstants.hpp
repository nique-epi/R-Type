#pragma once

#include <chrono>
#include <string_view>

namespace rtype::master {

/** @brief A day, and how long a sample of a heartbeat is kept: 7 days. */
constexpr std::chrono::seconds SECONDS_PER_DAY{86'400};
constexpr std::chrono::seconds SAMPLE_RETENTION = 7 * SECONDS_PER_DAY;

/** @brief The statements of the table `servers`. */
constexpr std::string_view CREATIONS_SINCE_QUERY = R"sql(
SELECT COUNT(*) FROM servers
WHERE created_by_address = :created_by_address AND created_at >= :since
)sql";
constexpr std::string_view INSERT_SERVER_STATEMENT = R"sql(
INSERT INTO servers (name, key_fingerprint, created_by_address, created_at)
VALUES (:name, :key_fingerprint, :created_by_address, :created_at)
RETURNING id
)sql";
constexpr std::string_view SERVER_OF_KEY_QUERY = R"sql(
SELECT id, maximum_games, maximum_players_per_game, address, udp_port,
       probe_secret
FROM servers WHERE key_fingerprint = :key_fingerprint
)sql";
constexpr std::string_view LAST_HEARTBEAT_OF_SERVER_QUERY =
    "SELECT last_heartbeat FROM servers WHERE id = :server_id";
constexpr std::string_view UPDATE_REGISTRATION_STATEMENT = R"sql(
UPDATE servers
SET name = :name, address = :address, udp_port = :udp_port,
    game_version = :game_version, protocol_version = :protocol_version,
    maximum_games = :maximum_games,
    maximum_players_per_game = :maximum_players_per_game,
    registered_at = :registered_at, probe_secret = :probe_secret,
    last_heartbeat = NULL, last_heartbeat_at = NULL, last_game_count = NULL,
    last_player_count = NULL, last_tick_p95_milliseconds = NULL,
    last_loss_percent = NULL, last_probe_answer_at = NULL
WHERE id = :server_id
)sql";
constexpr std::string_view UPDATE_LAST_HEARTBEAT_STATEMENT = R"sql(
UPDATE servers
SET last_heartbeat = :heartbeat, last_heartbeat_at = :taken_at,
    last_game_count = :game_count, last_player_count = :player_count,
    last_tick_p95_milliseconds = :tick_p95, last_loss_percent = :loss_percent,
    last_probe_answer_at =
      CASE WHEN :probe_answered THEN :taken_at ELSE last_probe_answer_at END
WHERE id = :server_id
)sql";

/** @brief The statements of the table `heartbeat_samples`. */
constexpr std::string_view INSERT_SAMPLE_STATEMENT = R"sql(
INSERT OR IGNORE INTO heartbeat_samples
  (server_id, taken_at, game_count, player_count, tick_p50_milliseconds,
   tick_p95_milliseconds, received_bytes_per_second, sent_bytes_per_second,
   loss_percent, average_ping_milliseconds, cpu_percent, used_memory_bytes,
   total_memory_bytes)
VALUES
  (:server_id, :taken_at, :game_count, :player_count, :tick_p50,
   :tick_p95, :received_bytes, :sent_bytes, :loss_percent, :average_ping,
   :cpu_percent, :used_memory, :total_memory)
)sql";
constexpr std::string_view ERASE_OLD_SAMPLES_STATEMENT =
    "DELETE FROM heartbeat_samples WHERE taken_at < :oldest";

/**
 * @brief How long a change of status is kept: as long as a sample, so the
 * history of a server holds both over the same days.
 */
constexpr std::chrono::seconds STATUS_CHANGE_RETENTION = SAMPLE_RETENTION;

/** @brief What the refresh of the statuses reads of each registered server. */
constexpr std::string_view WATCHED_SERVERS_QUERY = R"sql(
SELECT id, status, maintenance, registered_at, last_heartbeat_at,
       last_tick_p95_milliseconds, last_loss_percent, last_probe_answer_at
FROM servers WHERE registered_at IS NOT NULL ORDER BY id
)sql";
constexpr std::string_view UPDATE_STATUS_STATEMENT =
    "UPDATE servers SET status = :status WHERE id = :server_id";
constexpr std::string_view UPDATE_MAINTENANCE_STATEMENT =
    "UPDATE servers SET maintenance = :maintenance WHERE id = :server_id";

/** @brief A page of the registered servers, by id. */
constexpr std::string_view REGISTERED_SERVERS_QUERY = R"sql(
SELECT id, name, address, udp_port, game_version, protocol_version, status,
       maximum_games, maximum_players_per_game, COALESCE(last_game_count, 0),
       COALESCE(last_player_count, 0)
FROM servers WHERE registered_at IS NOT NULL
ORDER BY id LIMIT :limit OFFSET :offset
)sql";
constexpr std::string_view SERVER_EXISTS_QUERY =
    "SELECT 1 FROM servers WHERE id = :server_id";

/** @brief A page of the samples of a server, newest first. */
constexpr std::string_view SAMPLES_OF_SERVER_QUERY = R"sql(
SELECT taken_at, game_count, player_count, tick_p50_milliseconds,
       tick_p95_milliseconds, received_bytes_per_second, sent_bytes_per_second,
       loss_percent, average_ping_milliseconds, cpu_percent, used_memory_bytes,
       total_memory_bytes
FROM heartbeat_samples WHERE server_id = :server_id
ORDER BY taken_at DESC LIMIT :limit OFFSET :offset
)sql";

/** @brief The statements of the table `status_changes`. */
constexpr std::string_view INSERT_STATUS_CHANGE_STATEMENT = R"sql(
INSERT OR REPLACE INTO status_changes (server_id, changed_at, status)
VALUES (:server_id, :changed_at, :status)
)sql";
constexpr std::string_view ERASE_OLD_STATUS_CHANGES_STATEMENT =
    "DELETE FROM status_changes WHERE changed_at < :oldest";
constexpr std::string_view STATUS_CHANGES_OF_SERVER_QUERY = R"sql(
SELECT changed_at, status FROM status_changes WHERE server_id = :server_id
ORDER BY changed_at DESC LIMIT :limit OFFSET :offset
)sql";

/** @brief The parameters of those statements. */
constexpr std::string_view KEY_FINGERPRINT_PARAMETER = ":key_fingerprint";
constexpr std::string_view CREATED_BY_PARAMETER = ":created_by_address";
constexpr std::string_view CREATED_AT_PARAMETER = ":created_at";
constexpr std::string_view SINCE_PARAMETER = ":since";
constexpr std::string_view SERVER_NAME_PARAMETER = ":name";
constexpr std::string_view ADDRESS_PARAMETER = ":address";
constexpr std::string_view UDP_PORT_PARAMETER = ":udp_port";
constexpr std::string_view GAME_VERSION_PARAMETER = ":game_version";
constexpr std::string_view PROTOCOL_VERSION_PARAMETER = ":protocol_version";
constexpr std::string_view MAXIMUM_GAMES_PARAMETER = ":maximum_games";
constexpr std::string_view MAXIMUM_PLAYERS_PARAMETER =
    ":maximum_players_per_game";
constexpr std::string_view REGISTERED_AT_PARAMETER = ":registered_at";
constexpr std::string_view PROBE_SECRET_PARAMETER = ":probe_secret";
constexpr std::string_view PROBE_ANSWERED_PARAMETER = ":probe_answered";
constexpr std::string_view SERVER_ID_PARAMETER = ":server_id";
constexpr std::string_view HEARTBEAT_PARAMETER = ":heartbeat";
constexpr std::string_view TAKEN_AT_PARAMETER = ":taken_at";
constexpr std::string_view GAME_COUNT_PARAMETER = ":game_count";
constexpr std::string_view PLAYER_COUNT_PARAMETER = ":player_count";
constexpr std::string_view TICK_P50_PARAMETER = ":tick_p50";
constexpr std::string_view TICK_P95_PARAMETER = ":tick_p95";
constexpr std::string_view RECEIVED_BYTES_PARAMETER = ":received_bytes";
constexpr std::string_view SENT_BYTES_PARAMETER = ":sent_bytes";
constexpr std::string_view LOSS_PERCENT_PARAMETER = ":loss_percent";
constexpr std::string_view AVERAGE_PING_PARAMETER = ":average_ping";
constexpr std::string_view CPU_PERCENT_PARAMETER = ":cpu_percent";
constexpr std::string_view USED_MEMORY_PARAMETER = ":used_memory";
constexpr std::string_view TOTAL_MEMORY_PARAMETER = ":total_memory";
constexpr std::string_view OLDEST_PARAMETER = ":oldest";
constexpr std::string_view STATUS_PARAMETER = ":status";
constexpr std::string_view MAINTENANCE_PARAMETER = ":maintenance";
constexpr std::string_view CHANGED_AT_PARAMETER = ":changed_at";
constexpr std::string_view LIMIT_PARAMETER = ":limit";
constexpr std::string_view OFFSET_PARAMETER = ":offset";

/** @brief The column of the count CREATIONS_SINCE_QUERY returns. */
constexpr int CREATION_COUNT_COLUMN = 0;

/**
 * @brief The columns of SERVER_OF_KEY_QUERY; SERVER_ID_COLUMN is also the id
 * INSERT_SERVER_STATEMENT returns.
 */
constexpr int SERVER_ID_COLUMN = 0;
constexpr int MAXIMUM_GAMES_COLUMN = 1;
constexpr int MAXIMUM_PLAYERS_COLUMN = 2;
constexpr int SERVER_ADDRESS_COLUMN = 3;
constexpr int SERVER_UDP_PORT_COLUMN = 4;
constexpr int SERVER_PROBE_SECRET_COLUMN = 5;

/** @brief The column of the text LAST_HEARTBEAT_OF_SERVER_QUERY returns. */
constexpr int LAST_HEARTBEAT_COLUMN = 0;

/** @brief The columns of WATCHED_SERVERS_QUERY. */
constexpr int WATCHED_ID_COLUMN = 0;
constexpr int WATCHED_STATUS_COLUMN = 1;
constexpr int WATCHED_MAINTENANCE_COLUMN = 2;
constexpr int WATCHED_REGISTERED_AT_COLUMN = 3;
constexpr int WATCHED_HEARTBEAT_AT_COLUMN = 4;
constexpr int WATCHED_TICK_P95_COLUMN = 5;
constexpr int WATCHED_LOSS_PERCENT_COLUMN = 6;
constexpr int WATCHED_PROBE_ANSWER_AT_COLUMN = 7;

/** @brief The columns of REGISTERED_SERVERS_QUERY. */
constexpr int LISTED_ID_COLUMN = 0;
constexpr int LISTED_NAME_COLUMN = 1;
constexpr int LISTED_ADDRESS_COLUMN = 2;
constexpr int LISTED_UDP_PORT_COLUMN = 3;
constexpr int LISTED_GAME_VERSION_COLUMN = 4;
constexpr int LISTED_PROTOCOL_VERSION_COLUMN = 5;
constexpr int LISTED_STATUS_COLUMN = 6;
constexpr int LISTED_MAXIMUM_GAMES_COLUMN = 7;
constexpr int LISTED_MAXIMUM_PLAYERS_COLUMN = 8;
constexpr int LISTED_GAME_COUNT_COLUMN = 9;
constexpr int LISTED_PLAYER_COUNT_COLUMN = 10;

/** @brief The columns of SAMPLES_OF_SERVER_QUERY. */
constexpr int SAMPLE_TAKEN_AT_COLUMN = 0;
constexpr int SAMPLE_GAME_COUNT_COLUMN = 1;
constexpr int SAMPLE_PLAYER_COUNT_COLUMN = 2;
constexpr int SAMPLE_TICK_P50_COLUMN = 3;
constexpr int SAMPLE_TICK_P95_COLUMN = 4;
constexpr int SAMPLE_RECEIVED_BYTES_COLUMN = 5;
constexpr int SAMPLE_SENT_BYTES_COLUMN = 6;
constexpr int SAMPLE_LOSS_PERCENT_COLUMN = 7;
constexpr int SAMPLE_AVERAGE_PING_COLUMN = 8;
constexpr int SAMPLE_CPU_PERCENT_COLUMN = 9;
constexpr int SAMPLE_USED_MEMORY_COLUMN = 10;
constexpr int SAMPLE_TOTAL_MEMORY_COLUMN = 11;

/** @brief The columns of STATUS_CHANGES_OF_SERVER_QUERY. */
constexpr int CHANGED_AT_COLUMN = 0;
constexpr int CHANGE_STATUS_COLUMN = 1;

}  // namespace rtype::master
