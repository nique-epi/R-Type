#pragma once

#include <chrono>
#include <string_view>

/** @brief A second key name, and when the tests issue their keys. */
constexpr std::string_view LYON_KEY_NAME = "Lyon";
constexpr std::chrono::sys_seconds KEY_ISSUE_DATE{
    std::chrono::seconds{1'800'000'000}};

/** @brief The log of SQLite next to it, where a recent write may still be. */
constexpr std::string_view KEYS_DATABASE_LOG_SUFFIX = "-wal";
