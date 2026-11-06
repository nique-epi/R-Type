#pragma once

#include <chrono>
#include <string_view>

namespace rtype::storage {

/**
 * @brief How long a connection waits for a lock another connection holds: a
 * tool reading the file while the server runs, for instance.
 */
constexpr int DATABASE_BUSY_TIMEOUT_MILLISECONDS = 5000;

/**
 * @brief How long the switch to WAL pauses before it tries again while another
 * connection holds the lock it needs; SQLite answers that case at once, without
 * waiting the busy timeout.
 */
constexpr std::chrono::milliseconds WAL_SWITCH_RETRY_PAUSE{10};

/** @brief The settings every connection of the project runs with. */
constexpr std::string_view ENABLE_WAL_PRAGMA = "PRAGMA journal_mode = WAL";
constexpr std::string_view SYNCHRONOUS_FULL_PRAGMA =
    "PRAGMA synchronous = FULL";
constexpr std::string_view FOREIGN_KEYS_ON_PRAGMA = "PRAGMA foreign_keys = ON";

/** @brief The mode PRAGMA journal_mode answers once WAL is in force. */
constexpr std::string_view WAL_JOURNAL_MODE = "wal";

/** @brief Reads and writes the number of the last migration a file holds. */
constexpr std::string_view USER_VERSION_PRAGMA = "PRAGMA user_version";
constexpr std::string_view SET_USER_VERSION_PRAGMA = "PRAGMA user_version = ";

}  // namespace rtype::storage
