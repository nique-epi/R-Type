#pragma once

#include <chrono>
#include <string_view>

/** @brief A ticket of the shape the master issues: `rtt_` and 64 lowercase
 * hexadecimal digits; and the second it expires. */
constexpr std::string_view WELL_FORMED_TICKET =
    "rtt_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
constexpr std::chrono::sys_seconds TICKET_EXPIRY{
    std::chrono::seconds{1'800'021'600}};

/** @brief Tickets shaped almost right: the prefix of a session token, in
 * uppercase, 63 digits, 65 digits. */
constexpr std::string_view TICKET_WITH_A_TOKEN_PREFIX =
    "rts_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
constexpr std::string_view UPPERCASE_TICKET =
    "rtt_0123456789ABCDEF0123456789abcdef0123456789abcdef0123456789abcdef";
constexpr std::string_view SHORT_TICKET =
    "rtt_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcde";
constexpr std::string_view LONG_TICKET =
    "rtt_0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0";

/** @brief An answer of the master whose ticket is in uppercase. */
constexpr std::string_view UPPERCASE_TICKET_ANSWER =
    R"({"ticket":"rtt_0123456789ABCDEF0123456789abcdef0123456789abcdef0123456789abcdef","expiresAt":1800021600})";
