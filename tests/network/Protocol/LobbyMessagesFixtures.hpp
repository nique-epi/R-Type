#pragma once

#include <cstdint>
#include "LobbyState.hpp"

namespace rtype::network::testing {

/** @returns The player numbered @p playerNumber, not ready, with a nickname of
 * sixteen bytes. */
[[nodiscard]] LobbyPlayer lobbyPlayer(std::uint8_t playerNumber);

/** @returns A room of MAXIMUM_PLAYERS players numbered 1 to 4, none ready. */
[[nodiscard]] LobbyState fullLobby();

}  // namespace rtype::network::testing
