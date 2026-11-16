#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>

/** @brief The date ACE opens its session, and dates measured from it. */
constexpr std::chrono::sys_seconds SESSION_OPENED_AT{
    std::chrono::seconds{1'800'000'000}};
constexpr std::chrono::sys_seconds JUST_BEFORE_A_WEEK =
    SESSION_OPENED_AT + std::chrono::days{7} - std::chrono::seconds{1};
constexpr std::chrono::sys_seconds A_WEEK_LATER =
    SESSION_OPENED_AT + std::chrono::days{7};
constexpr std::chrono::sys_seconds TWO_DAYS_LATER =
    SESSION_OPENED_AT + std::chrono::days{2};
constexpr std::chrono::sys_seconds EIGHT_DAYS_LATER =
    SESSION_OPENED_AT + std::chrono::days{8};
constexpr std::chrono::sys_seconds HALF_A_DAY_LATER =
    SESSION_OPENED_AT + std::chrono::hours{12};

/** @brief The address the tests open sessions from. */
constexpr std::string_view SERVICE_CLIENT_ADDRESS = "203.0.113.7";

/** @brief How a test reads what the database keeps of a password. */
constexpr std::string_view KEPT_PASSWORD_QUERY =
    "SELECT password_hash FROM users WHERE nickname = :nickname";
constexpr std::string_view KEPT_PASSWORD_NICKNAME_PARAMETER = ":nickname";
constexpr int KEPT_PASSWORD_COLUMN = 0;
constexpr std::size_t KEPT_PASSWORD_BYTES = 128;
