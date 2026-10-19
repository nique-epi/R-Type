#pragma once

#include <chrono>

namespace rtype::server::testing {

/**
 * @brief The longest gap the protocol allows between two LobbyState while a
 * game waits for players: the server sends one at least every 500 ms.
 */
constexpr std::chrono::milliseconds LONGEST_LOBBY_STATE_GAP{500};

}  // namespace rtype::server::testing
