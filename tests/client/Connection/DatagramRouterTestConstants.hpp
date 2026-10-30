#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/**
 * @brief The senders of the router tests: the game server, a master server on
 * another port of the same address, and a host nobody listens to.
 */
constexpr std::string_view GAME_SERVER_ADDRESS = "192.0.2.30";
constexpr std::uint16_t GAME_SERVER_PORT = 4242;
constexpr std::uint16_t MASTER_SERVER_PORT = 4300;
constexpr std::string_view STRANGER_ADDRESS = "198.51.100.7";
constexpr std::uint16_t STRANGER_PORT = 4242;

/** @brief The byte that tells the datagrams of a test apart. */
constexpr std::byte FIRST_TAG{1};
constexpr std::byte SECOND_TAG{2};
constexpr std::byte THIRD_TAG{3};

}  // namespace rtype::client::testing
