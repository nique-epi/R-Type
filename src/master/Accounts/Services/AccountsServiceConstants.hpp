#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

namespace rtype::master {

/**
 * @brief How many accounts one address may create in USER_CREATION_WINDOW,
 * as many as the servers it may create: every creation hashes a password, so
 * the quota also bounds the work a client can ask for.
 */
constexpr std::int64_t MAXIMUM_USERS_PER_ADDRESS = 5;
constexpr std::chrono::seconds USER_CREATION_WINDOW = std::chrono::hours{1};

/**
 * @brief How many sign-ins from one address may fail in SIGN_IN_FAILURE_WINDOW:
 * past it, every sign-in from that address is refused before its password is
 * checked, until the oldest failure is out of the window.
 */
constexpr std::int64_t MAXIMUM_SIGN_IN_FAILURES = 5;
constexpr std::chrono::seconds SIGN_IN_FAILURE_WINDOW =
    std::chrono::minutes{15};

/**
 * @brief How long a session lives, and how often its use moves its expiry:
 * the defaults of Better Auth, 7 days, moved again at most once a day, so a
 * player who plays every week never signs in again, and a session read a
 * hundred times a day is written once.
 */
constexpr std::chrono::seconds SESSION_LIFETIME = std::chrono::days{7};
constexpr std::chrono::seconds SESSION_REFRESH_AGE = std::chrono::days{1};

/** @brief The words of the lines the sign-ins write in the journal. */
constexpr std::string_view SIGNED_IN_TEXT = " signed in from ";
constexpr std::string_view SIGN_IN_AS_TEXT = "sign-in as ";
constexpr std::string_view SIGN_IN_FROM_TEXT = " from ";
constexpr std::string_view WRONG_CREDENTIALS_TEXT =
    " refused: wrong nickname or password";
constexpr std::string_view TOO_MANY_FAILURES_TEXT =
    " refused: too many failed sign-ins from this address";

}  // namespace rtype::master
