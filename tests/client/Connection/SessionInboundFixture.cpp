#include "SessionInboundFixture.hpp"
#include <cstddef>
#include <cstdint>
#include "ConnectionAccepted.hpp"
#include "DatagramCodec.hpp"
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "Reception.hpp"
#include "ServerSessionFixtures.hpp"
#include "ServerSessionTestConstants.hpp"

namespace rtype::client::testing {

void SessionInboundFixture::openSession() {
  network::PacketHeader acceptance;
  acceptance.sessionToken = SESSION_TOKEN;
  acceptance.sequence = ACCEPTANCE_SEQUENCE;
  inbound_.open(acceptance);
}

Reception SessionInboundFixture::receiveWithToken(std::uint64_t token) {
  network::PacketHeader header;
  header.sessionToken = token;
  header.sequence = FIRST_SEQUENCE;
  return receive(header);
}

Reception SessionInboundFixture::receiveWithSequence(std::uint16_t sequence) {
  network::PacketHeader header;
  header.sessionToken = SESSION_TOKEN;
  header.sequence = sequence;
  return receive(header);
}

Reception SessionInboundFixture::receive(const network::PacketHeader& header) {
  return inbound_.receive(network::IncomingDatagram{
      .sender = server(),
      .payload = network::DatagramCodec::encode(
          header, network::ConnectionAccepted{.playerNumber =
                                                  ACCEPTED_PLAYER_NUMBER})});
}

std::size_t SessionInboundFixture::wrongTokenCount() const {
  return inbound_.wrongTokenCount();
}

}  // namespace rtype::client::testing
