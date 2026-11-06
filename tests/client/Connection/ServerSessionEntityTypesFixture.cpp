#include "ServerSessionEntityTypesFixture.hpp"
#include <cstdint>
#include <utility>
#include <vector>
#include "ConnectionAccepted.hpp"
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "Disconnection.hpp"
#include "EntityTypeEntry.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ServerSession.hpp"
#include "ServerSessionEntityTypesTestConstants.hpp"
#include "ServerSessionFixtures.hpp"
#include "ServerSessionTestConstants.hpp"
#include "SessionEntityTypes.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

void ServerSessionEntityTypesFixture::connectAndGetAcceptedWith(
    std::vector<network::EntityTypeEntry> types) {
  session_.connect(server(), NICKNAME, rtype::network::ANY_GAME_CHOICE);
  network::PacketHeader header;
  header.sessionToken = SESSION_TOKEN;
  link_.deliver(network::IncomingDatagram{
      .sender = server(),
      .payload = network::DatagramCodec::encode(
          header,
          network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                                      .types = std::move(types)})});
  update();
}

void ServerSessionEntityTypesFixture::getAcceptedAgainWith(
    std::vector<network::EntityTypeEntry> types) {
  link_.deliver(network::IncomingDatagram{
      .sender = server(),
      .payload = network::DatagramCodec::encode(
          sessionHeader(REPEATED_ACCEPTANCE_SEQUENCE),
          network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                                      .types = std::move(types)})});
  update();
}

void ServerSessionEntityTypesFixture::update(engine::Duration elapsed) {
  session_.update(elapsed);
}

const SessionEntityTypes& ServerSessionEntityTypesFixture::entityTypes() const {
  return entityTypes_;
}

ServerSession& ServerSessionEntityTypesFixture::session() { return session_; }

std::vector<std::uint64_t>
ServerSessionEntityTypesFixture::disconnectionTokens() const {
  std::vector<std::uint64_t> tokens;
  for (const network::OutgoingDatagram& datagram : link_.sent()) {
    if (holds<network::Disconnection>(datagram)) {
      tokens.push_back(
          network::DatagramCodec::decodeHeader(datagram.payload).sessionToken);
    }
  }
  return tokens;
}

}  // namespace rtype::client::testing
