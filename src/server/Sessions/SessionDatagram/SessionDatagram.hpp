#pragma once

#include <cstddef>
#include <span>
#include "ConnectedPlayer.hpp"
#include "Endpoint.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"

namespace rtype::server {

/**
 * @brief Gives the header of the next datagram of the session of
 * @p recipient: its session token and its next sequence, which then moves on
 * by one, wrapping from 65535 to 0. The ticks since a datagram was sent to
 * @p recipient start again from 0, since one is about to be.
 *
 * Every datagram the server sends a connected player takes its header here,
 * whatever its message, so the sequences a player receives grow by one from 0
 * across every kind of message: the client drops a datagram that is not newer
 * than the newest one it read (see the Datagram section of the protocol page).
 */
[[nodiscard]] network::PacketHeader nextSessionHeader(
    ConnectedPlayer& recipient);

/**
 * @brief Gives the header of the next datagram to the player of @p roster who
 * sends from @p recipient, as nextSessionHeader() does.
 * @returns A header without session, token 0 and sequence 0, when no player
 * of @p roster sends from @p recipient; a client in a session drops it.
 */
[[nodiscard]] network::PacketHeader sessionHeaderFor(
    PlayerRoster& roster, const network::Endpoint& recipient);

/**
 * @returns A datagram to @p recipient that holds @p payload as one message of
 * type @p type, under the header nextSessionHeader() gives.
 * @throws DatagramTooLargeException when @p payload is over
 * MESSAGE_PAYLOAD_LIMIT.
 */
[[nodiscard]] network::OutgoingDatagram sessionDatagram(
    ConnectedPlayer& recipient, network::MessageType type,
    std::span<const std::byte> payload);

/**
 * @returns A datagram to @p recipient that holds @p message, under the header
 * nextSessionHeader() gives.
 * @throws What encodeMessage() throws for @p message.
 */
template <network::EncodableMessage Message>
[[nodiscard]] network::OutgoingDatagram sessionDatagram(
    ConnectedPlayer& recipient, const Message& message) {
  return sessionDatagram(recipient, Message::TYPE,
                         network::encodeMessage(message));
}

}  // namespace rtype::server
