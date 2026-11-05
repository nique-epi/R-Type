#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::server::testing {

/** @brief The name a server announces while server-name is left as it is,
 * from server.cfg.example. */
constexpr std::string_view NAME_OF_A_SERVER_LEFT_AS_SHIPPED = "R-Type server";

/** @brief A line that names the server, and the name it gives. */
constexpr std::string_view NAME_THE_SERVER_LYON_LINE = "set server-name Lyon";
constexpr std::string_view LYON = "Lyon";

/** @brief A line that gives the server a name of 33 bytes, one too many. */
constexpr std::string_view NAME_TOO_LONG_LINE =
    "set server-name abcdefghijklmnopqrstuvwxyz0123456";

/** @brief Two games, so eight seats; a player in the first, which starts,
 * leaves the four seats of the second free. */
constexpr std::size_t TWO_GAMES_HOSTED = 2;
constexpr std::uint16_t SEATS_OF_TWO_GAMES_HOSTED = 8;
constexpr std::uint16_t SEATS_OF_THE_SECOND_GAME = 4;

}  // namespace rtype::server::testing
