#pragma once

#include <chrono>
#include <string_view>

/** @brief A secret of the shape the master draws: `rtd_` and 64 lowercase
 * hexadecimal digits; and one with the prefix of a session token. */
constexpr std::string_view WELL_FORMED_SECRET =
    "rtd_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
constexpr std::string_view SECRET_WITH_A_TOKEN_PREFIX =
    "rts_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";

/** @brief A short code as the master keeps it, and the ways a player may type
 * it: in lowercase, without its dash, with spaces around. */
constexpr std::string_view KEPT_USER_CODE = "KQFB-WZPM";
constexpr std::string_view LOWERCASE_USER_CODE = "kqfb-wzpm";
constexpr std::string_view USER_CODE_WITHOUT_DASH = "KQFBWZPM";

/** @brief Texts that are no short code: a vowel, a digit, a letter short, a
 * letter too many, nothing. */
constexpr std::string_view USER_CODE_WITH_A_VOWEL = "KQFB-WZPA";
constexpr std::string_view USER_CODE_WITH_A_DIGIT = "KQFB-WZP1";
constexpr std::string_view SHORT_USER_CODE = "KQFB-WZP";
constexpr std::string_view LONG_USER_CODE = "KQFB-WZPMB";
constexpr std::string_view EMPTY_USER_CODE{};

/** @brief The page that validates a link, with its code, and the dates of
 * the link. */
constexpr std::string_view LINK_PAGE = "https://rtype.app/link";
constexpr std::string_view LINK_PAGE_WITH_CODE =
    "https://rtype.app/link?code=KQFB-WZPM";
constexpr std::chrono::sys_seconds LINK_CREATED_AT{
    std::chrono::seconds{1'800'000'000}};
constexpr std::chrono::sys_seconds LINK_EXPIRES_AT{
    std::chrono::seconds{1'800'000'600}};
constexpr std::chrono::seconds LINK_INTERVAL{5};

/** @brief A body of a decision whose word is no decision, and where it is. */
constexpr std::string_view MAYBE_DECISION = R"({"decision":"maybe"})";
constexpr std::string_view DECISION_POINTER = "/decision";

/** @brief An answer of the master whose interval is 0 seconds. */
constexpr std::string_view NO_INTERVAL_STATE =
    R"({"userCode":"KQFB-WZPM","status":"pending","createdAt":1800000000,"expiresAt":1800000600,"interval":0})";
constexpr std::string_view INTERVAL_POINTER = "/interval";
