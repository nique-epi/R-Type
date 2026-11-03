#include "GameResultsFixtures.hpp"
#include <cstddef>
#include <vector>
#include "DatagramCodec.hpp"
#include "Disconnection.hpp"
#include "GameInstanceFixtures.hpp"
#include "GameInstanceTestConstants.hpp"
#include "IncomingDatagram.hpp"
#include "LeaderboardSamples.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"

namespace rtype::server::testing {

namespace {

network::PacketHeader headerFor(std::size_t index) {
  network::PacketHeader header = sessionHeader(index);
  header.sequence = FIRST_SEQUENCE;
  return header;
}

}  // namespace

network::IncomingDatagram leaderboardRequestFrom(std::size_t index) {
  return network::IncomingDatagram{
      .sender = playerEndpoint(index),
      .payload = network::DatagramCodec::encode(headerFor(index),
                                                someLeaderboardRequest())};
}

network::IncomingDatagram disconnectionThenLeaderboardRequestFrom(
    std::size_t index) {
  std::vector<std::byte> datagram = network::DatagramCodec::encode(
      headerFor(index), network::Disconnection{});
  const std::vector<std::byte> request = leaderboardRequestFrom(index).payload;
  datagram.insert(datagram.end(), request.begin() + network::PACKET_HEADER_SIZE,
                  request.end());
  return network::IncomingDatagram{.sender = playerEndpoint(index),
                                   .payload = datagram};
}

}  // namespace rtype::server::testing
