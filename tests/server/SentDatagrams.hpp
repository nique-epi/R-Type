#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>
#include "CarriedMessage.hpp"
#include "Endpoint.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "WorldState.hpp"

namespace rtype::server::testing {

/**
 * @returns Where the player at @p index sends from in the server tests:
 * PLAYER_ADDRESS, and the port at @p index in FIVE_PLAYER_PORTS.
 */
[[nodiscard]] network::Endpoint playerEndpoint(std::size_t index);

/**
 * @returns The datagrams of @p sent that go to @p destination, oldest first.
 */
[[nodiscard]] std::vector<network::OutgoingDatagram> sentTo(
    std::span<const network::OutgoingDatagram> sent,
    const network::Endpoint& destination);

/**
 * @returns The messages @p datagram carries, in order, as a client reads
 * them: a message sent on the ordered channel is read out of its
 * OrderedMessage, the others as they are.
 */
[[nodiscard]] std::vector<CarriedMessage> carriedMessagesOf(
    const network::OutgoingDatagram& datagram);

/**
 * @returns The type of the only message @p datagram carries. The test fails
 * when the datagram carries another number of messages.
 */
[[nodiscard]] network::MessageType onlyMessageTypeOf(
    const network::OutgoingDatagram& datagram);

/**
 * @returns The world state the only message of @p datagram holds, a
 * DeltaWorldState without reference, which lists every entity: as the sender
 * writes the first state of a player, and every state while the player
 * acknowledges none. The test fails when the datagram holds another number of
 * messages, a state with a reference, or one part of several.
 */
[[nodiscard]] network::WorldState fullStateIn(
    const network::OutgoingDatagram& datagram);

/** @returns The header of @p datagram. */
[[nodiscard]] network::PacketHeader headerOf(
    const network::OutgoingDatagram& datagram);

/** @returns How many messages @p datagram holds, an OrderedMessage counting
 * for one. */
[[nodiscard]] std::size_t messageCountIn(
    const network::OutgoingDatagram& datagram);

/** @returns Whether @p datagram carries a message of type @p type. */
[[nodiscard]] bool holdsMessageOfType(const network::OutgoingDatagram& datagram,
                                      network::MessageType type);

/**
 * @returns The sequence of the last of @p datagrams that holds a message of
 * type @p type; nothing when none does.
 */
[[nodiscard]] std::optional<std::uint16_t> lastSequenceHolding(
    std::span<const network::OutgoingDatagram> datagrams,
    network::MessageType type);

/**
 * @returns The only message of @p datagram, read as a Message. The test fails
 * when the datagram holds another number of messages, or one of another type.
 */
template <network::DecodableMessage Message>
[[nodiscard]] Message onlyMessageIn(const network::OutgoingDatagram& datagram) {
  const std::vector<CarriedMessage> carried = carriedMessagesOf(datagram);
  EXPECT_EQ(carried.size(), 1U);
  EXPECT_EQ(carried.at(0).type, static_cast<std::uint8_t>(Message::TYPE));
  return network::decodeMessage<Message>(carried.at(0).payload);
}

/** @returns Every message of type Message that @p datagram holds, in order. */
template <network::DecodableMessage Message>
[[nodiscard]] std::vector<Message> messagesIn(
    const network::OutgoingDatagram& datagram) {
  std::vector<Message> messages;
  for (const CarriedMessage& carried : carriedMessagesOf(datagram)) {
    if (carried.type == static_cast<std::uint8_t>(Message::TYPE)) {
      messages.push_back(network::decodeMessage<Message>(carried.payload));
    }
  }
  return messages;
}

}  // namespace rtype::server::testing
