#pragma once

#include <cstddef>
#include <vector>
#include "IncomingDatagram.hpp"
#include "LobbyState.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::server::testing {

/** @returns A datagram from the player at @p index saying it sits at the
 * roulette table. */
[[nodiscard]] network::IncomingDatagram casinoPlaceFrom(std::size_t index);

/** @returns The LobbyState messages of @p datagrams, oldest first. */
[[nodiscard]] std::vector<network::LobbyState> lobbyStatesIn(
    const std::vector<network::OutgoingDatagram>& datagrams);

}  // namespace rtype::server::testing
