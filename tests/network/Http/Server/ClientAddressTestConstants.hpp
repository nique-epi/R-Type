#pragma once

#include <string_view>

/** @brief The TCP peer of a request that comes through the proxy on the
 * machine, and of one that reaches the server from elsewhere. */
constexpr std::string_view PROXY_ON_THE_MACHINE = "127.0.0.1";
constexpr std::string_view DIRECT_CLIENT = "198.51.100.20";

/** @brief An X-Forwarded-For whose client sent two addresses before the
 * proxy appended the one it saw, with the blanks a proxy may leave: two
 * separators, so the first and the last one cut it at different places. */
constexpr std::string_view CHAIN_ENDING_IN_THE_SEEN_CLIENT =
    "198.51.100.4, 198.51.100.5,  203.0.113.9 ";
constexpr std::string_view SEEN_CLIENT = "203.0.113.9";

/** @brief An X-Forwarded-For naming one client, as Caddy writes it. */
constexpr std::string_view ONE_FORWARDED_CLIENT = "203.0.113.7";

/** @brief An X-Forwarded-For whose last entry names no address. */
constexpr std::string_view CHAIN_ENDING_IN_A_BLANK = "203.0.113.9, ";
