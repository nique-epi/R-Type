#pragma once

#include <chrono>
#include <string_view>

/** @brief The name of the key the tests issue, and when they issue it. */
constexpr std::string_view PARIS_KEY_NAME = "Paris 1";
constexpr std::string_view LYON_KEY_NAME = "Lyon";
constexpr std::chrono::sys_seconds KEY_ISSUE_DATE{
    std::chrono::seconds{1'800'000'000}};

/** @brief The database file the tests give the keys, in their folder. */
constexpr std::string_view KEYS_DATABASE_FILE = "master.db";

/** @brief The log of SQLite next to it, where a recent write may still be. */
constexpr std::string_view KEYS_DATABASE_LOG_SUFFIX = "-wal";
