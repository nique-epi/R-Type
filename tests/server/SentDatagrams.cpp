#include "SentDatagrams.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>
#include "CarriedMessage.hpp"
#include "DatagramCodec.hpp"
#include "DeltaWorldState.hpp"
#include "Endpoint.hpp"
#include "MessageFrame.hpp"
#include "MessageType.hpp"
#include "OrderedMessage.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagramsTestConstants.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "WorldState.hpp"

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

std::vector<CarriedMessage> carriedMessagesOf(
    const network::OutgoingDatagram& datagram) {
  std::vector<CarriedMessage> carried;
  for (const network::MessageFrame& frame :
       network::DatagramCodec::splitMessages(datagram.payload)) {
    if (frame.type ==
        static_cast<std::uint8_t>(network::MessageType::OrderedMessage)) {
      const auto envelope =
          network::decodeMessage<network::OrderedMessage>(frame.payload);
      carried.push_back(CarriedMessage{.type = envelope.innerType,
                                       .payload = envelope.payload});
    } else {
      carried.push_back(
          CarriedMessage{.type = frame.type,
                         .payload = std::vector<std::byte>(
                             frame.payload.begin(), frame.payload.end())});
    }
  }
  return carried;
}

network::MessageType onlyMessageTypeOf(
    const network::OutgoingDatagram& datagram) {
  const std::vector<CarriedMessage> carried = carriedMessagesOf(datagram);
  EXPECT_EQ(carried.size(), 1U);
  return static_cast<network::MessageType>(carried.at(0).type);
}

network::WorldState fullStateIn(const network::OutgoingDatagram& datagram) {
  const auto delta = onlyMessageIn<network::DeltaWorldState>(datagram);
  EXPECT_EQ(delta.baseDistance, network::NO_REFERENCE_DISTANCE);
  EXPECT_EQ(delta.partCount, ONE_PART);
  return network::WorldState{
      .tick = delta.tick, .players = delta.players, .entities = delta.changed};
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
      carriedMessagesOf(datagram), [type](const CarriedMessage& carried) {
        return carried.type == static_cast<std::uint8_t>(type);
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
