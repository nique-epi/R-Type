#include "ServerSessionFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>
#include "ConnectionAccepted.hpp"
#include "ConnectionRequest.hpp"
#include "ConnectionStatus.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "EntityMessage.hpp"
#include "IncomingDatagram.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerInput.hpp"
#include "ServerSession.hpp"
#include "ServerSessionFixtures.hpp"
#include "ServerSessionTestConstants.hpp"
#include "TimeConstants.hpp"
#include "WaitingRoom.hpp"
#include "WorldState.hpp"
#include "WorldStateCodec.hpp"

namespace rtype::client::testing {

void ServerSessionFixture::connect() { session_.connect(server(), NICKNAME); }

void ServerSessionFixture::deliverBytes(std::vector<std::byte> datagram) {
  link_.deliver(network::IncomingDatagram{.sender = server(),
                                          .payload = std::move(datagram)});
}

void ServerSessionFixture::update(engine::Duration elapsed) {
  entityMessages_.advance(elapsed);
  session_.update(elapsed);
}

void ServerSessionFixture::connectAndGetAccepted() {
  connect();
  network::PacketHeader header;
  header.sessionToken = SESSION_TOKEN;
  deliver(network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER},
          server(), header);
  update();
}

ConnectionStatus::Phase ServerSessionFixture::phase() const {
  return session_.status().phase;
}

ServerSession& ServerSessionFixture::session() { return session_; }

WaitingRoom& ServerSessionFixture::waitingRoom() { return waitingRoom_; }

const std::vector<network::OutgoingDatagram>& ServerSessionFixture::sent()
    const {
  return link_.sent();
}

network::ConnectionRequest ServerSessionFixture::lastRequest() const {
  return onlyMessageOf<network::ConnectionRequest>(link_.sent().back());
}

void ServerSessionFixture::accept() {
  connect();
  deliverInSession(
      network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER},
      ACCEPTANCE_SEQUENCE);
  update();
}

void ServerSessionFixture::deliverWorldStateInSession(
    const network::WorldState& state, std::uint16_t sequence) {
  const std::vector<std::vector<std::byte>> payloads =
      network::WorldStateCodec::encode(state);
  deliverBytes(network::DatagramCodec::encode(sessionHeader(sequence),
                                              network::MessageType::WorldState,
                                              payloads.front()));
}

std::vector<EntityMessage> ServerSessionFixture::drainedEntityMessages() {
  std::vector<EntityMessage> drained;
  entityMessages_.drainEntityMessages(drained);
  return drained;
}

network::PacketHeader ServerSessionFixture::sessionHeader(
    std::uint16_t sequence) {
  network::PacketHeader header;
  header.sessionToken = SESSION_TOKEN;
  header.sequence = sequence;
  return header;
}

const std::optional<network::Endpoint>& ServerSessionFixture::listenedTo()
    const {
  return link_.listenedTo();
}

network::PlayerInput ServerSessionFixture::lastInput() const {
  return inputOf(link_.sent().back());
}

network::PacketHeader ServerSessionFixture::lastHeader() const {
  return network::DatagramCodec::decodeHeader(link_.sent().back().payload);
}

}  // namespace rtype::client::testing
