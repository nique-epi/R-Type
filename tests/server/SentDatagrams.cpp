#include "SentDatagrams.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "MessageFrame.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

network::Endpoint playerEndpoint(std::size_t index) {
  return network::Endpoint{.address = std::string{PLAYER_ADDRESS},
                           .port = FIVE_PLAYER_PORTS.at(index)};
}

std::vector<network::OutgoingDatagram> sentTo(
    std::span<const network::OutgoingDatagram> sent,
    const network::Endpoint& destination) {
  std::vector<network::OutgoingDatagram> datagrams;
  for (const network::OutgoingDatagram& datagram : sent) {
    if (datagram.destination == destination) {
      datagrams.push_back(datagram);
    }
  }
  return datagrams;
}

network::MessageType onlyMessageTypeOf(
    const network::OutgoingDatagram& datagram) {
  const std::vector<network::MessageFrame> frames =
      network::DatagramCodec::splitMessages(datagram.payload);
  EXPECT_EQ(frames.size(), 1U);
  return static_cast<network::MessageType>(frames.at(0).type);
}

network::PacketHeader headerOf(const network::OutgoingDatagram& datagram) {
  return network::DatagramCodec::decodeHeader(datagram.payload);
}

std::size_t messageCountIn(const network::OutgoingDatagram& datagram) {
  return network::DatagramCodec::splitMessages(datagram.payload).size();
}

bool holdsMessageOfType(const network::OutgoingDatagram& datagram,
                        network::MessageType type) {
  return std::ranges::any_of(
      network::DatagramCodec::splitMessages(datagram.payload),
      [type](const network::MessageFrame& frame) {
        return frame.type == static_cast<std::uint8_t>(type);
      });
}

std::optional<std::uint16_t> lastSequenceHolding(
    std::span<const network::OutgoingDatagram> datagrams,
    network::MessageType type) {
  std::optional<std::uint16_t> sequence;
  for (const network::OutgoingDatagram& datagram : datagrams) {
    if (holdsMessageOfType(datagram, type)) {
      sequence = headerOf(datagram).sequence;
    }
  }
  return sequence;
}

}  // namespace rtype::server::testing
