#pragma once

#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include "ActionSet.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "MessageFrame.hpp"
#include "MessagePayload.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerInput.hpp"

namespace rtype::client::testing {

/** @returns The server the tests connect to: SERVER_ADDRESS, SERVER_PORT. */
network::Endpoint server();

/** @returns Another sender on the address of the server: SERVER_ADDRESS,
 * OTHER_SENDER_PORT. */
network::Endpoint otherSender();

/** @returns The message of type Message that @p datagram holds, its only
 * one. */
template <typename Message>
Message onlyMessageOf(const network::OutgoingDatagram& datagram) {
  const std::vector<network::MessageFrame> frames =
      network::DatagramCodec::splitMessages(datagram.payload);
  EXPECT_EQ(frames.size(), 1);
  EXPECT_EQ(frames.front().type, static_cast<std::uint8_t>(Message::TYPE));
  return network::decodeMessage<Message>(frames.front().payload);
}

/** @returns Whether @p datagram holds a message of type Message first. */
template <typename Message>
bool holds(const network::OutgoingDatagram& datagram) {
  const std::vector<network::MessageFrame> frames =
      network::DatagramCodec::splitMessages(datagram.payload);
  return !frames.empty() &&
         frames.front().type == static_cast<std::uint8_t>(Message::TYPE);
}

/** @returns The input @p datagram holds, its only message. */
network::PlayerInput inputOf(const network::OutgoingDatagram& datagram);

/** @returns The actions of a player who holds up only. */
ActionSet upHeld();

}  // namespace rtype::client::testing
