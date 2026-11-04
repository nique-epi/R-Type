#pragma once

#include <functional>
#include "Heartbeat.hpp"
#include "MessageOrigin.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief Adds @p elapsed, the real time since the previous tick started, to
 * ConnectedPlayer::sinceHeartbeat of each player of @p roster. Called when a
 * tick starts, before anything is sent in it.
 */
void addTimeSinceHeartbeat(PlayerRoster& roster, engine::Duration elapsed);

/**
 * @brief Sends each player of @p roster whose sinceHeartbeat reached
 * TIME_BETWEEN_HEARTBEATS a Heartbeat its RoundTripMeter builds at @p now,
 * then RoundTrips, the round trip of every player of @p roster measured; its
 * sinceHeartbeat then starts over. Two datagrams, since sessionDatagram()
 * puts one message in each.
 * @param now The time on the clock of the sessions: the steady clock the
 * network thread stamps each datagram with (see IncomingDatagram::receivedAt).
 * @param send Called for each datagram to send; must not be empty.
 */
void sendHeartbeats(PlayerRoster& roster, engine::Duration now,
                    const std::function<void(network::OutgoingDatagram)>& send);

/**
 * @brief Hands @p heartbeat to the RoundTripMeter of the player of @p roster
 * who sends from the sender of @p origin, with the time its datagram arrived;
 * a sender @p roster does not hold changes nothing.
 */
void readHeartbeat(PlayerRoster& roster, const network::MessageOrigin& origin,
                   const network::Heartbeat& heartbeat);

}  // namespace rtype::server
