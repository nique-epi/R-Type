#pragma once

#include <cstdint>
#include "LobbyState.hpp"

namespace rtype::network::testing {

/** @returns Player @p playerNumber, not ready, with a sixteen-byte nickname
 * and no map vote. */
[[nodiscard]] LobbyPlayer lobbyPlayer(std::uint8_t playerNumber);

/** @returns The largest room: four players with sixteen-byte nicknames, each
 * voting for the last map, and MAXIMUM_LOBBY_MAPS maps with sixteen-byte
 * names. */
[[nodiscard]] LobbyState fullLobby();

}  // namespace rtype::network::testing
