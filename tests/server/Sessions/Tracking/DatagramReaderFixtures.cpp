#include "DatagramReaderFixtures.hpp"
#include <cstdint>
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "SentDatagrams.hpp"
#include "ServerDatagrams.hpp"

namespace rtype::server::testing {

network::IncomingDatagram heartbeatOfSequence(std::uint16_t sequence) {
  return network::IncomingDatagram{
      .sender = playerEndpoint(0),
      .payload =
          heartbeatDatagram(network::PacketHeader{.sequence = sequence})};
}

}  // namespace rtype::server::testing
