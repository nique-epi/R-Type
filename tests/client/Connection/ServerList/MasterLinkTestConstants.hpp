#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief The server the tests create and register on the master. */
constexpr std::string_view LINKED_SERVER_NAME = "Paris 1";
constexpr std::string_view LINKED_SERVER_ADDRESS = "203.0.113.7";
constexpr std::uint16_t LINKED_SERVER_PORT = 4242;
constexpr std::string_view LINKED_SERVER_GAME_VERSION = "0.4.0";
constexpr std::uint16_t LINKED_SERVER_MAXIMUM_GAMES = 8;
constexpr std::uint8_t LINKED_SERVER_PLAYERS_PER_GAME = 4;

/** @brief How long a test waits for the thread of the link to read, longer
 * than the timeouts of MasterClient, and how often it looks. */
constexpr std::chrono::seconds READING_DEADLINE{12};
constexpr std::chrono::milliseconds POLL_STEP{10};

}  // namespace rtype::client::testing
