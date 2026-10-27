#include "ServerConnectionFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include "ConnectionAccepted.hpp"
#include "ConnectionPhase.hpp"
#include "ConnectionRequest.hpp"
#include "ConnectionStatus.hpp"
#include "DatagramCodec.hpp"
#include "EntityMessage.hpp"
#include "IncomingDatagram.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "ServerConnection.hpp"
#include "ServerConnectionFixtures.hpp"
#include "ServerConnectionTestConstants.hpp"
#include "TimeConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateCodec.hpp"

namespace rtype::client::testing {

void ServerConnectionFixture::connect() {
  connection_.connect(server(), NICKNAME);
}

void ServerConnectionFixture::deliverBytes(std::vector<std::byte> datagram) {
  link_.deliver(network::IncomingDatagram{.sender = server(),
                                          .payload = std::move(datagram)});
}

void ServerConnectionFixture::update(engine::Duration elapsed) {
  connection_.update(elapsed);
}

void ServerConnectionFixture::connectAndGetAccepted() {
  connect();
  network::PacketHeader header;
  header.sessionToken = SESSION_TOKEN;
  deliver(network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER},
          server(), header);
  update();
}

ConnectionPhase ServerConnectionFixture::phase() const {
  return connection_.status().phase;
}

ServerConnection& ServerConnectionFixture::connection() { return connection_; }

const std::vector<network::OutgoingDatagram>& ServerConnectionFixture::sent()
    const {
  return link_.sent();
}

network::ConnectionRequest ServerConnectionFixture::lastRequest() const {
  return onlyMessageOf<network::ConnectionRequest>(link_.sent().back());
}

void ServerConnectionFixture::accept() {
  connect();
  deliverInSession(
      network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER},
      ACCEPTANCE_SEQUENCE);
  update();
}

void ServerConnectionFixture::deliverWorldStateInSession(
    const network::WorldState& state, std::uint16_t sequence) {
  const std::vector<std::vector<std::byte>> payloads =
      network::WorldStateCodec::encode(state);
  deliverBytes(network::DatagramCodec::encode(sessionHeader(sequence),
                                              network::MessageType::WorldState,
                                              payloads.front()));
}

std::vector<EntityMessage> ServerConnectionFixture::drainedEntityMessages() {
  std::vector<EntityMessage> drained;
  connection_.drainEntityMessages(drained);
  return drained;
}

network::PacketHeader ServerConnectionFixture::sessionHeader(
    std::uint16_t sequence) {
  network::PacketHeader header;
  header.sessionToken = SESSION_TOKEN;
  header.sequence = sequence;
  return header;
}

network::PlayerInput ServerConnectionFixture::lastInput() const {
  return inputOf(link_.sent().back());
}

network::PacketHeader ServerConnectionFixture::lastHeader() const {
  return network::DatagramCodec::decodeHeader(link_.sent().back().payload);
}

}  // namespace rtype::client::testing
