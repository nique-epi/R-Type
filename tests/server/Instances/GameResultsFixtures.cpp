#include "GameResultsFixtures.hpp"
#include <cstddef>
#include "DatagramCodec.hpp"
#include "GameInstanceFixtures.hpp"
#include "IncomingDatagram.hpp"
#include "LeaderboardSamples.hpp"
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
  return disconnectionThen(index, leaderboardRequestFrom(index));
}

}  // namespace rtype::server::testing
