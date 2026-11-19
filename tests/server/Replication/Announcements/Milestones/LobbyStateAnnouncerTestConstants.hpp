#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

namespace rtype::server::testing {

/**
 * @brief The longest gap the protocol allows between two LobbyState while a
 * game waits for players: the server sends one at least every 500 ms.
 */
constexpr std::chrono::milliseconds LONGEST_LOBBY_STATE_GAP{500};

/** @brief The maps the announcer lists, in order, and the number of the
 * second, from 1. */
constexpr std::string_view FIRST_ANNOUNCED_MAP = "OPEN SPACE";
constexpr std::string_view SECOND_ANNOUNCED_MAP = "THE PIT";
constexpr std::uint8_t SECOND_ANNOUNCED_MAP_NUMBER = 2;

}  // namespace rtype::server::testing
