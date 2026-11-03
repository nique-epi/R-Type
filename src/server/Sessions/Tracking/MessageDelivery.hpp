#pragma once

#include <functional>
#include "EncodedMessage.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief Sends @p message once to every player of @p roster, each datagram
 * under the next session header of its player: a message whose loss a later
 * state message corrects.
 * @param send Called for each datagram to send; must not be empty.
 */
void sendToEveryPlayer(
    PlayerRoster& roster,
    const std::function<void(network::OutgoingDatagram)>& send,
    const network::EncodedMessage& message);

/**
 * @brief Gives @p message to every player of @p roster as a guaranteed
 * message, then sends each of them its guaranteed messages due, @p message
 * among them, at once. Only the players of @p roster now get it: a player who
 * joins later does not.
 * @param send Called for each datagram to send; must not be empty.
 * @throws DatagramTooLargeException when @p message does not fit a datagram
 * alone; no player gets it then.
 */
void guaranteeToEveryPlayer(
    PlayerRoster& roster,
    const std::function<void(network::OutgoingDatagram)>& send,
    const network::EncodedMessage& message);

/**
 * @brief Makes the guaranteed messages of every player of @p roster @p elapsed
 * older, and drops those that reach GUARANTEED_MESSAGE_LIFETIME. Called when a
 * tick starts, with the real time since the previous one started, as the time
 * since a player was last sent a datagram is counted (see
 * ConnectedPlayer::sinceSent): a stall of the server ages them too, since the
 * client waits in real time.
 */
void ageGuaranteedMessages(PlayerRoster& roster, engine::Duration elapsed);

/**
 * @brief Sends each player of @p roster its guaranteed messages due again,
 * grouped in as few datagrams as they fit, each under the next session header
 * of the player with GUARANTEED_FLAG set. Called once at the end of every tick.
 * @param send Called for each datagram to send; must not be empty.
 */
void resendGuaranteedMessages(
    PlayerRoster& roster,
    const std::function<void(network::OutgoingDatagram)>& send);

}  // namespace rtype::server
