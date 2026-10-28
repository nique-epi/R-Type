#pragma once

#include <functional>
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

/**
 * @brief Sends a Heartbeat to every player of @p roster the server sent
 * nothing for TICKS_PER_HEARTBEAT ticks, whatever sent it, so its client knows
 * the server is still there in a state that repeats no message; then counts the
 * tick that ends for every player. The Heartbeat takes its header from
 * sessionDatagram(), which starts the count over for that player.
 * @param send Called for each datagram to send; must not be empty.
 */
void sendHeartbeats(PlayerRoster& roster,
                    const std::function<void(network::OutgoingDatagram)>& send);

}  // namespace rtype::server
