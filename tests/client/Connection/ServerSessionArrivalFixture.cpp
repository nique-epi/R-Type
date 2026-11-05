#include "ServerSessionArrivalFixture.hpp"
#include <chrono>
#include <optional>
#include "ConnectionAccepted.hpp"
#include "DatagramCodec.hpp"
#include "Heartbeat.hpp"
#include "IncomingDatagram.hpp"
#include "PacketHeader.hpp"
#include "RoundTripFigures.hpp"
#include "ServerSessionFixtures.hpp"
#include "ServerSessionTestConstants.hpp"
#include "SessionEntityTypesFixtures.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

void ServerSessionArrivalFixture::openSession() {
  session_.connect(server(), NICKNAME);
  network::PacketHeader header;
  header.sessionToken = SESSION_TOKEN;
  link_.deliver(network::IncomingDatagram{
      .sender = server(),
      .payload = network::DatagramCodec::encode(
          header,
          network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                                      .types = drawnTypes()})});
  update(engine::Duration::zero());
}

void ServerSessionArrivalFixture::update(engine::Duration elapsed) {
  clock_.advance(elapsed);
  session_.update(elapsed);
}

network::Heartbeat ServerSessionArrivalFixture::lastSentHeartbeat() const {
  return onlyMessageOf<network::Heartbeat>(link_.sent().back());
}

void ServerSessionArrivalFixture::deliverStamped(
    const network::Heartbeat& heartbeat, std::chrono::nanoseconds receivedAt) {
  network::PacketHeader header;
  header.sessionToken = SESSION_TOKEN;
  header.sequence = FIRST_SEQUENCE;
  link_.deliver(network::IncomingDatagram{
      .sender = server(),
      .payload = network::DatagramCodec::encode(header, heartbeat),
      .receivedAt = receivedAt});
  update(engine::Duration::zero());
}

std::optional<network::RoundTripFigures>
ServerSessionArrivalFixture::ownRoundTrip() const {
  return roundTrips_.roundTripOf(ACCEPTED_PLAYER_NUMBER);
}

}  // namespace rtype::client::testing
