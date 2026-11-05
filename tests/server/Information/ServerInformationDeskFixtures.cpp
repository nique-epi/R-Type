#include "ServerInformationDeskFixtures.hpp"
#include <cstdint>
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "SentDatagrams.hpp"
#include "ServerDatagrams.hpp"
#include "ServerInformationDeskTestConstants.hpp"

rtype::network::IncomingDatagram informationRequestArriving(
    std::uint32_t sentAt) {
  return rtype::network::IncomingDatagram{
      .sender = rtype::server::testing::playerEndpoint(0),
      .payload =
          informationRequestDatagram(rtype::network::PacketHeader{}, sentAt),
      .receivedAt = rtype::server::testing::REQUEST_ARRIVAL};
}

rtype::network::IncomingDatagram cutShort(
    rtype::network::IncomingDatagram datagram) {
  datagram.payload.pop_back();
  return datagram;
}
