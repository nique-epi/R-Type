#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::server::testing {

/** @brief The server the messages describe. */
constexpr std::string_view LINKED_SERVER_NAME = "Paris 1";
constexpr std::uint16_t LINKED_SERVER_PORT = 4242;
constexpr std::uint16_t LINKED_SERVER_GAMES = 8;
constexpr std::uint64_t LINKED_SERVER_PROBE_SECRET = 0x0FED'CBA9'8765'4321;

/** @brief The bounds of a probe secret the master accepts: 1 to 2^63 - 1,
 * what a column INTEGER of SQLite holds. */
constexpr std::uint64_t PROBE_SECRET_FLOOR = 1;
constexpr std::uint64_t PROBE_SECRET_CEILING = 9'223'372'036'854'775'807;

/** @brief A token of a probe a heartbeat carries. */
constexpr std::uint64_t PROBE_TOKEN_OF_A_HEARTBEAT = 0x1357'9BDF'2468'ACE0;

/** @brief What the server announces of the protocol and of its games. */
constexpr std::uint8_t ANNOUNCED_PROTOCOL_VERSION = 11;
constexpr std::uint8_t ANNOUNCED_PLAYERS_PER_GAME = 4;

/** @brief A median tick a heartbeat carries, in milliseconds. */
constexpr double CARRIED_MEDIAN_TICK_MILLISECONDS = 2.5;

}  // namespace rtype::server::testing
