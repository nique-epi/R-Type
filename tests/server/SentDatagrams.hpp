#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "MessageFrame.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"

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
 * @returns The type of the only message of @p datagram. The test fails when
 * the datagram holds another number of messages.
 */
[[nodiscard]] network::MessageType onlyMessageTypeOf(
    const network::OutgoingDatagram& datagram);

/**
 * @returns The only message of @p datagram, read as a Message. The test fails
 * when the datagram holds another number of messages, or one of another type.
 */
template <network::DecodableMessage Message>
[[nodiscard]] Message onlyMessageIn(const network::OutgoingDatagram& datagram) {
  const std::vector<network::MessageFrame> frames =
      network::DatagramCodec::splitMessages(datagram.payload);
  EXPECT_EQ(frames.size(), 1U);
  EXPECT_EQ(frames.at(0).type, static_cast<std::uint8_t>(Message::TYPE));
  return network::decodeMessage<Message>(frames.at(0).payload);
}

}  // namespace rtype::server::testing
