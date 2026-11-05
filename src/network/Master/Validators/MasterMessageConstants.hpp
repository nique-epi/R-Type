#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>

namespace rtype::network {

/** @brief The members of the registration of a server, in JSON. */
constexpr std::string_view NAME_MEMBER = "name";
constexpr std::string_view ADDRESS_MEMBER = "address";
constexpr std::string_view UDP_PORT_MEMBER = "udpPort";
constexpr std::string_view GAME_VERSION_MEMBER = "gameVersion";
constexpr std::string_view PROTOCOL_VERSION_MEMBER = "protocolVersion";
constexpr std::string_view MAXIMUM_GAMES_MEMBER = "maximumGames";
constexpr std::string_view MAXIMUM_PLAYERS_PER_GAME_MEMBER =
    "maximumPlayersPerGame";

/** @brief The members of a heartbeat and of each game it lists, in JSON. */
constexpr std::string_view GAMES_MEMBER = "games";
constexpr std::string_view MEASURES_MEMBER = "measures";
constexpr std::string_view NUMBER_MEMBER = "number";
constexpr std::string_view STATE_MEMBER = "state";
constexpr std::string_view DIFFICULTY_MEMBER = "difficulty";
constexpr std::string_view PLAYERS_MEMBER = "players";

/** @brief The members of the measures of a heartbeat, in JSON. */
constexpr std::string_view TICK_P50_MEMBER = "tickP50Milliseconds";
constexpr std::string_view TICK_P95_MEMBER = "tickP95Milliseconds";
constexpr std::string_view RECEIVED_BYTES_MEMBER = "receivedBytesPerSecond";
constexpr std::string_view SENT_BYTES_MEMBER = "sentBytesPerSecond";
constexpr std::string_view LOSS_PERCENT_MEMBER = "lossPercent";
constexpr std::string_view AVERAGE_PING_MEMBER = "averagePingMilliseconds";

/** @brief The member of the answer to a heartbeat that lists the orders. */
constexpr std::string_view COMMANDS_MEMBER = "commands";

/** @brief The members of a created server, beside NAME_MEMBER. */
constexpr std::string_view ID_MEMBER = "id";
constexpr std::string_view KEY_MEMBER = "key";

/** @brief The lowest and the highest id of a game server. */
constexpr std::uint64_t FIRST_SERVER_ID = 1;
constexpr std::uint64_t HIGHEST_SERVER_ID =
    std::numeric_limits<std::int64_t>::max();

/**
 * @brief The key of a game server: SERVER_KEY_PREFIX, so a key that leaks in a
 * log or a repository is recognized, then SERVER_KEY_BYTES random bytes in
 * lowercase hexadecimal, SERVER_KEY_SIZE characters in all.
 */
constexpr std::string_view SERVER_KEY_PREFIX = "rtk_";
constexpr std::size_t SERVER_KEY_BYTES = 32;
constexpr std::size_t HEX_DIGITS_PER_BYTE = 2;
constexpr std::size_t SERVER_KEY_SIZE =
    SERVER_KEY_PREFIX.size() + (SERVER_KEY_BYTES * HEX_DIGITS_PER_BYTE);
constexpr std::string_view SERVER_KEY_DIGITS = "0123456789abcdef";

/** @brief How each HostedGameState is written, in the order of the enum. */
constexpr std::string_view WAITING_STATE_TEXT = "waiting";
constexpr std::string_view STARTING_STATE_TEXT = "starting";
constexpr std::string_view PLAYING_STATE_TEXT = "playing";
constexpr std::string_view FINISHED_STATE_TEXT = "finished";
constexpr std::string_view FULL_STATE_TEXT = "full";
constexpr auto HOSTED_GAME_STATE_TEXTS = std::to_array<std::string_view>(
    {WAITING_STATE_TEXT, STARTING_STATE_TEXT, PLAYING_STATE_TEXT,
     FINISHED_STATE_TEXT, FULL_STATE_TEXT});

/** @brief How each ListedGameState is written, in the order of the enum. */
constexpr std::string_view IN_GAME_STATE_TEXT = "inGame";
constexpr auto LISTED_GAME_STATE_TEXTS = std::to_array<std::string_view>(
    {WAITING_STATE_TEXT, IN_GAME_STATE_TEXT, FULL_STATE_TEXT});

/** @brief How each ServerStatus is written, in the order of the enum. */
constexpr std::string_view ONLINE_STATUS_TEXT = "online";
constexpr std::string_view DEGRADED_STATUS_TEXT = "degraded";
constexpr std::string_view OFFLINE_STATUS_TEXT = "offline";
constexpr std::string_view MAINTENANCE_STATUS_TEXT = "maintenance";
constexpr auto SERVER_STATUS_TEXTS = std::to_array<std::string_view>(
    {ONLINE_STATUS_TEXT, DEGRADED_STATUS_TEXT, OFFLINE_STATUS_TEXT,
     MAINTENANCE_STATUS_TEXT});

/**
 * @brief The members of a listed server beside those of a registration, of a
 * sample, and of a dated status; a date is written in seconds since 1970
 * UTC.
 */
constexpr std::string_view STATUS_MEMBER = "status";
constexpr std::string_view GAME_COUNT_MEMBER = "gameCount";
constexpr std::string_view TAKEN_AT_MEMBER = "takenAt";
constexpr std::string_view SINCE_MEMBER = "since";

/** @brief The member that holds the items of each list. */
constexpr std::string_view SERVERS_MEMBER = "servers";
constexpr std::string_view SAMPLES_MEMBER = "samples";
constexpr std::string_view STATUSES_MEMBER = "statuses";

/** @brief The member of a listed game that counts its players. */
constexpr std::string_view PLAYER_COUNT_MEMBER = "playerCount";

/** @brief The fewest players a listed game may hold. */
constexpr std::uint64_t MINIMUM_LISTED_PLAYERS = 0;

/** @brief The longest name of a game, in bytes of UTF-8. */
constexpr std::size_t MAXIMUM_GAME_NAME_SIZE = 16;

/**
 * @brief The fewest and the most games a server may host, and the number of
 * its first game. A server hosts 8 today (DEFAULT_HOSTED_GAME_COUNT of the
 * server); a heartbeat of 64 games of four players, every nickname and
 * difficulty of 16 bytes and every measure taken, is 9.8 KiB of JSON.
 */
constexpr std::uint16_t MINIMUM_HOSTED_GAMES = 1;
constexpr std::uint16_t MAXIMUM_HOSTED_GAMES = 64;
constexpr std::uint16_t FIRST_HOSTED_GAME_NUMBER = 1;

/** @brief The fewest players a game of a server may take. */
constexpr std::uint8_t MINIMUM_PLAYERS_PER_GAME = 1;

/** @brief The lowest and the highest version of the UDP protocol. */
constexpr std::uint8_t LOWEST_PROTOCOL_VERSION = 0;
constexpr std::uint8_t HIGHEST_PROTOCOL_VERSION =
    std::numeric_limits<std::uint8_t>::max();

/** @brief The highest value of a measure, and of a share of lost datagrams. */
constexpr double MAXIMUM_MEASURE = std::numeric_limits<double>::max();
constexpr double MAXIMUM_LOSS_PERCENT = 100.0;

/**
 * @brief How often a game server tells the master its state when nothing
 * changes; the master counts the heartbeats a server missed in this interval.
 */
constexpr std::chrono::milliseconds MASTER_HEARTBEAT_INTERVAL{5'000};

}  // namespace rtype::network
