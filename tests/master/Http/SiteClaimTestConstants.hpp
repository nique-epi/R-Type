#pragma once

#include <array>
#include <string_view>

/** @brief The headers the web site sends with a creation, as it writes them.
 */
constexpr std::string_view SITE_KEY_HEADER_NAME = "X-Site-Key";
constexpr std::string_view SITE_CLIENT_ADDRESS_HEADER_NAME =
    "X-Site-Client-Address";

/** @brief What a test reads as the status of a creation the master did not
 * answer. */
constexpr int NO_ANSWER_STATUS = 0;

/** @brief The secret the master of a test is given, and one it is not. */
constexpr std::string_view TEST_SITE_KEY =
    "5d1f0c6a9b3e4872a1d0c4f7e6b59a3821f4c0de7b6a5d4938271605f4e3d2c1";
constexpr std::string_view WRONG_SITE_KEY =
    "5d1f0c6a9b3e4872a1d0c4f7e6b59a3821f4c0de7b6a5d4938271605f4e3d2c2";
constexpr std::string_view NO_SITE_KEY;

/** @brief Two visitors of the site (addresses of documentation, RFC 5737),
 * and one that reaches it over IPv6 (RFC 3849). */
constexpr std::string_view FIRST_VISITOR = "198.51.100.23";
constexpr std::string_view SECOND_VISITOR = "203.0.113.9";
constexpr std::string_view IPV6_VISITOR = "2001:db8::17";

/** @brief The longest text of an address, 45 characters, and the same with
 * one more digit. */
constexpr std::string_view LONGEST_ADDRESS =
    "0000:0000:0000:0000:0000:ffff:255.255.255.255";
constexpr std::string_view ADDRESS_ONE_CHARACTER_TOO_LONG =
    "00000:0000:0000:0000:0000:ffff:255.255.255.255";

/** @brief Texts that name no address: nothing, a host name, a number past
 * 255, two addresses, a single colon between two groups, a letter no base
 * sixteen has. */
constexpr std::string_view NOT_AN_ADDRESS = "visitor.example";
constexpr std::array<std::string_view, 6> TEXTS_THAT_ARE_NO_ADDRESS{
    std::string_view{},        NOT_AN_ADDRESS, "198.51.100.256",
    "198.51.100.23, 10.0.0.1", "2001:db8",     "2001:db8::g7"};
