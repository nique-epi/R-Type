#include "StopOrder.hpp"
#include <string>
#include "DatagramCodec.hpp"
#include "Disconnection.hpp"
#include "Endpoint.hpp"
#include "NetworkConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "SupervisedGame.hpp"

namespace rtype::server {

network::OutgoingDatagram stopOrderTo(const SupervisedGame& game) {
  return network::OutgoingDatagram{
      .destination =
          network::Endpoint{
              .address = std::string{network::LOOPBACK_IPV4_ADDRESS},
              .port = game.port},
      .payload = network::DatagramCodec::encode(
          network::PacketHeader{.sessionToken = game.linkToken},
          network::Disconnection{})};
}

}  // namespace rtype::server
