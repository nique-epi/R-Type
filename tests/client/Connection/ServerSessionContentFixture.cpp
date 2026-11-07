#include "ServerSessionContentFixture.hpp"
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "ConnectionAccepted.hpp"
#include "ConnectionPhase.hpp"
#include "ContentAnnouncement.hpp"
#include "ContentPackRoute.hpp"
#include "ContentPackTestFiles.hpp"
#include "DatagramCodec.hpp"
#include "Disconnection.hpp"
#include "Endpoint.hpp"
#include "FakePackServer.hpp"
#include "IncomingDatagram.hpp"
#include "NetworkConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ServerSession.hpp"
#include "ServerSessionContentTestConstants.hpp"
#include "ServerSessionFixtures.hpp"
#include "ServerSessionTestConstants.hpp"
#include "SessionEntityTypes.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

network::Endpoint ServerSessionContentFixture::loopbackServer() {
  return network::Endpoint{
      .address = std::string{network::LOOPBACK_IPV4_ADDRESS},
      .port = SERVER_PORT};
}

void ServerSessionContentFixture::connectAndGetAcceptedWith(
    const network::ContentAnnouncement& content) {
  session_.connect(loopbackServer(), NICKNAME, rtype::network::ANY_GAME_CHOICE);
  network::PacketHeader header;
  header.sessionToken = SESSION_TOKEN;
  link_.deliver(network::IncomingDatagram{
      .sender = loopbackServer(),
      .payload = network::DatagramCodec::encode(
          header,
          network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                                      .content = content})});
  update();
}

void ServerSessionContentFixture::getAcceptedAgainWith(
    const network::ContentAnnouncement& content) {
  link_.deliver(network::IncomingDatagram{
      .sender = loopbackServer(),
      .payload = network::DatagramCodec::encode(
          sessionHeader(REPEATED_ACCEPTANCE_SEQUENCE),
          network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                                      .content = content})});
  update();
}

network::ContentAnnouncement ServerSessionContentFixture::serveTheLancerPackAs(
    const std::string& body) {
  const std::vector<std::byte> pack = lancerPack();
  const network::ContentAnnouncement content{
      .fingerprint = fingerprintOf(pack),
      .size = static_cast<std::uint32_t>(body.size()),
      .port = 0};
  supervisor_ = std::make_unique<FakePackServer>(
      network::contentPackPathOf(content.fingerprint), body);
  return network::ContentAnnouncement{.fingerprint = content.fingerprint,
                                      .size = content.size,
                                      .port = supervisor_->port()};
}

FakePackServer& ServerSessionContentFixture::supervisor() {
  return *supervisor_;
}

void ServerSessionContentFixture::updateWhileLoading() {
  const auto deadline = std::chrono::steady_clock::now() + LOADING_WAIT;
  while (session_.status().phase == ConnectionPhase::LoadingContent &&
         std::chrono::steady_clock::now() < deadline) {
    std::this_thread::sleep_for(LOADING_FRAME);
    update(LOADING_FRAME);
  }
}

void ServerSessionContentFixture::update(engine::Duration elapsed) {
  session_.update(elapsed);
}

const SessionEntityTypes& ServerSessionContentFixture::entityTypes() const {
  return entityTypes_;
}

ServerSession& ServerSessionContentFixture::session() { return session_; }

std::vector<std::uint64_t> ServerSessionContentFixture::disconnectionTokens()
    const {
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
