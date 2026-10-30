#pragma once

#include <functional>
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief Adds @p elapsed, the real time since the previous tick started, to
 * ConnectedPlayer::sinceSent of each player of @p roster. Called when a tick
 * starts, before anything is sent in it.
 */
void addTimeSinceSent(PlayerRoster& roster, engine::Duration elapsed);

/**
 * @brief Sends a Heartbeat to every player of @p roster the server sent nothing
 * for SILENCE_BEFORE_HEARTBEAT, whatever sent it, so its client knows the
 * server is still there in a state that repeats no message. The Heartbeat takes
 * its header from sessionDatagram(), which starts that time over for the
 * player.
 * @param send Called for each datagram to send; must not be empty.
 */
void sendHeartbeats(PlayerRoster& roster,
                    const std::function<void(network::OutgoingDatagram)>& send);

}  // namespace rtype::server
