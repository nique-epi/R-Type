#pragma once

#include <cstddef>
#include "ConnectedPlayer.hpp"
#include "GameInstance.hpp"
#include "IncomingDatagram.hpp"

/** @brief Connects the player at @p index to @p game, under the session token
 * of that index.
 * @returns The player, now seated in the game. */
rtype::server::ConnectedPlayer& seatPlayerIn(rtype::server::GameInstance& game,
                                             std::size_t index);

/** @returns A Disconnection from the player at @p index, as its client sends
 * it. */
[[nodiscard]] rtype::network::IncomingDatagram disconnectionFrom(
    std::size_t index);
