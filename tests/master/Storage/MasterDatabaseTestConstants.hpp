#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>

/** @brief The migrations a master of version 1 applied. */
constexpr std::size_t VERSION_ONE_MIGRATIONS = 1;

/** @brief When the server of a file at version 1 registered and last beat. */
constexpr std::chrono::sys_seconds VERSION_ONE_DATE{
    std::chrono::seconds{1'800'000'000}};

/**
 * @brief "Paris 1" registered, as a master of version 1 wrote it, its last
 * heartbeat and its dates bound.
 */
constexpr std::string_view INSERT_VERSION_ONE_SERVER = R"sql(
INSERT INTO servers (name, key_fingerprint, created_by_address, created_at,
                     address, udp_port, game_version, protocol_version,
                     maximum_games, maximum_players_per_game, registered_at,
                     last_heartbeat, last_heartbeat_at)
VALUES ('Paris 1', 'fingerprint of a version 1 key', '198.51.100.7', :date,
        '203.0.113.7', 4242, '0.4.0', 2, 8, 4, :date, :heartbeat, :date)
)sql";
constexpr std::string_view VERSION_ONE_DATE_PARAMETER = ":date";
constexpr std::string_view VERSION_ONE_HEARTBEAT_PARAMETER = ":heartbeat";

/**
 * @brief Records an answer to a probe at the last heartbeat of every server,
 * as a master of this version records it for a heartbeat that answers one.
 */
constexpr std::string_view MARK_PROBE_ANSWERED_STATEMENT =
    "UPDATE servers SET last_probe_answer_at = last_heartbeat_at";

/** @brief A last heartbeat that is no JSON. */
constexpr std::string_view UNREADABLE_HEARTBEAT = "not json";
