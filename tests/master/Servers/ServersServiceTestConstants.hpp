#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

/** @brief When the first heartbeat of a test arrives, and later dates. */
constexpr std::chrono::sys_seconds FIRST_HEARTBEAT_DATE{
    std::chrono::seconds{1'800'000'000}};
constexpr std::chrono::seconds ONE_SECOND{1};
constexpr std::chrono::seconds TEST_DAY{86'400};
constexpr std::chrono::seconds SEVEN_DAYS = 7 * TEST_DAY;
constexpr std::chrono::seconds EIGHT_DAYS = 8 * TEST_DAY;

/** @brief A new name of the server, sent by a second registration. */
constexpr std::string_view RENAMED_SERVER = "Paris 2";

/** @brief Bounds lower than those of "Paris 1", which its heartbeat breaks. */
constexpr std::uint16_t ONE_GAME = 1;
constexpr std::uint8_t ONE_PLAYER_PER_GAME = 1;

/** @brief The servers that beat at once, and the heartbeats each sends. */
constexpr std::size_t PARALLEL_SERVERS = 8;
constexpr std::size_t HEARTBEATS_PER_SERVER = 50;

/** @brief What the tests read of the tables, to check what was written. */
constexpr std::string_view SAMPLE_COUNT_QUERY =
    "SELECT COUNT(*) FROM heartbeat_samples";
constexpr std::string_view SERVER_NAME_QUERY =
    "SELECT name FROM servers WHERE id = :id";
constexpr std::string_view LAST_HEARTBEAT_QUERY =
    "SELECT last_heartbeat FROM servers WHERE id = :id";
constexpr std::string_view TESTED_ID_PARAMETER = ":id";
constexpr int ONLY_COLUMN = 0;

/** @brief The samples one heartbeat, then two seconds of them, leave. */
constexpr std::int64_t ONE_SAMPLE = 1;
constexpr std::int64_t TWO_SAMPLES = 2;
