#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::server::testing {

/** @brief The server the messages describe. */
constexpr std::string_view LINKED_SERVER_NAME = "Paris 1";
constexpr std::uint16_t LINKED_SERVER_PORT = 4242;
constexpr std::uint16_t LINKED_SERVER_GAMES = 8;

/** @brief What the server announces of the protocol and of its games. */
constexpr std::uint8_t ANNOUNCED_PROTOCOL_VERSION = 8;
constexpr std::uint8_t ANNOUNCED_PLAYERS_PER_GAME = 4;

/** @brief A median tick a heartbeat carries, in milliseconds. */
constexpr double CARRIED_MEDIAN_TICK_MILLISECONDS = 2.5;

}  // namespace rtype::server::testing
