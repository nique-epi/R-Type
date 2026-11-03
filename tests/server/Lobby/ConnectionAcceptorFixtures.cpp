#include "ConnectionAcceptorFixtures.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "ByteWriter.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "MessageFrame.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

network::Endpoint requesterEndpoint(std::size_t requester) {
  return network::Endpoint{.address = std::string{PLAYER_ADDRESS},
                           .port = FIVE_PLAYER_PORTS.at(requester)};
}

std::vector<std::byte> emptyNicknameRequestDatagram() {
  network::ByteWriter payload;
  payload.writeUint64(0);
  payload.writeString("");
  return network::DatagramCodec::encode(network::PacketHeader{},
                                        network::MessageType::ConnectionRequest,
                                        payload.bytes());
}

std::uint8_t messageTypeOf(const network::OutgoingDatagram& datagram) {
  const std::vector<network::MessageFrame> frames =
      network::DatagramCodec::splitMessages(datagram.payload);
  EXPECT_EQ(frames.size(), 1);
  return frames.front().type;
}

std::string nicknameOf(std::size_t requester) {
  return "Pilot" + std::to_string(requester);
}

}  // namespace rtype::server::testing
