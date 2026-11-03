#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "MessagePayload.hpp"
#include "OutgoingDatagram.hpp"

namespace rtype::server::testing {

/** @returns The address of the requester at @p requester: PLAYER_ADDRESS, on
 * its port of FIVE_PLAYER_PORTS. */
network::Endpoint requesterEndpoint(std::size_t requester);

/**
 * @returns A datagram holding a ConnectionRequest whose nickname is empty,
 * which its reader refuses.
 */
std::vector<std::byte> emptyNicknameRequestDatagram();

/** @returns The type of the only message of @p datagram. */
std::uint8_t messageTypeOf(const network::OutgoingDatagram& datagram);

/** @returns The only message of @p datagram, read as a Message. */
template <typename Message>
Message messageOf(const network::OutgoingDatagram& datagram) {
  EXPECT_EQ(messageTypeOf(datagram), static_cast<std::uint8_t>(Message::TYPE));
  return network::decodeMessage<Message>(
      network::DatagramCodec::splitMessages(datagram.payload).front().payload);
}

/** @returns A nickname of its own for the requester at @p requester. */
std::string nicknameOf(std::size_t requester);

}  // namespace rtype::server::testing
