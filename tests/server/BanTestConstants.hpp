#pragma once

#include <chrono>
#include <string_view>

namespace rtype::server::testing {

/** @brief A banned nickname, in capitals and in small letters, which are one
 * nickname, and a nickname no ban names. */
constexpr std::string_view BANNED_NICKNAME = "ACE";
constexpr std::string_view BANNED_NICKNAME_IN_SMALL_LETTERS = "ace";
constexpr std::string_view FREE_NICKNAME = "NOVA";

/** @brief A banned address of the documentation range, and another one. */
constexpr std::string_view BANNED_ADDRESS = "203.0.113.7";
constexpr std::string_view FREE_ADDRESS = "203.0.113.8";

/** @brief When the tests ban, and when a ban of an hour ends. */
constexpr std::chrono::sys_seconds BAN_MOMENT{
    std::chrono::seconds{1'800'000'000}};
constexpr std::chrono::sys_seconds HOUR_BAN_END =
    BAN_MOMENT + std::chrono::hours{1};
constexpr std::chrono::seconds ONE_SECOND{1};
/** @brief A ban that ended long before any test runs. */
constexpr std::chrono::sys_seconds LONG_AGO{std::chrono::seconds{1}};
/** @brief Long after every ban of an hour ended: a ban for good still bars. */
constexpr std::chrono::sys_seconds LONG_AFTER =
    BAN_MOMENT + std::chrono::years{10};

}  // namespace rtype::server::testing
