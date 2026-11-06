#include "GameResultsFixtures.hpp"
#include <cstddef>
#include <vector>
#include "DatagramCodec.hpp"
#include "Disconnection.hpp"
#include "GameInstanceFixtures.hpp"
#include "IncomingDatagram.hpp"
#include "LeaderboardSamples.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"

namespace rtype::server::testing {

network::IncomingDatagram leaderboardRequestFrom(std::size_t index) {
  return network::IncomingDatagram{
      .sender = playerEndpoint(index),
      .payload = network::DatagramCodec::encode(firstSessionHeader(index),
                                                someLeaderboardRequest())};
}

network::IncomingDatagram disconnectionThenLeaderboardRequestFrom(
    std::size_t index) {
  std::vector<std::byte> datagram = network::DatagramCodec::encode(
      firstSessionHeader(index), network::Disconnection{});
  const std::vector<std::byte> request = leaderboardRequestFrom(index).payload;
  datagram.insert(datagram.end(), request.begin() + network::PACKET_HEADER_SIZE,
                  request.end());
  return network::IncomingDatagram{.sender = playerEndpoint(index),
                                   .payload = datagram};
}

}  // namespace rtype::server::testing
