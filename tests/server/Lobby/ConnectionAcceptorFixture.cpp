#include "ConnectionAcceptorFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "ConnectionAcceptor.hpp"
#include "ConnectionAcceptorFixtures.hpp"
#include "ConnectionChallenge.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ServerDatagrams.hpp"
#include "TimeConstants.hpp"

namespace rtype::server::testing {

void ConnectionAcceptorFixture::receive(const network::Endpoint& sender,
                                        std::vector<std::byte> datagram,
                                        engine::Duration now) {
  const network::IncomingDatagram received{.sender = sender,
                                           .payload = std::move(datagram)};
  acceptor_.receive(std::span{&received, 1}, now);
}

std::uint64_t ConnectionAcceptorFixture::challengeCookie(
    const network::Endpoint& sender, const std::string& nickname) {
  receive(sender, requestDatagramOf(nickname, 0));
  return messageOf<network::ConnectionChallenge>(lastAnswer()).cookie;
}

std::uint64_t ConnectionAcceptorFixture::challengeCookie(
    const network::Endpoint& sender) {
  receive(sender, requestDatagram(0));
  return messageOf<network::ConnectionChallenge>(lastAnswer()).cookie;
}

void ConnectionAcceptorFixture::connect(const network::Endpoint& sender) {
  receive(sender, requestDatagram(challengeCookie(sender)));
}

void ConnectionAcceptorFixture::connectAs(const network::Endpoint& sender,
                                          const std::string& nickname) {
  receive(sender,
          requestDatagramOf(nickname, challengeCookie(sender, nickname)));
}

const std::vector<network::OutgoingDatagram>&
ConnectionAcceptorFixture::answers() const {
  return answers_;
}

const network::OutgoingDatagram& ConnectionAcceptorFixture::lastAnswer() const {
  return answers_.back();
}

PlayerRoster& ConnectionAcceptorFixture::roster() { return roster_; }

const std::vector<std::pair<network::Endpoint, std::uint64_t>>&
ConnectionAcceptorFixture::admitted() const {
  return admitted_;
}

ConnectionAcceptor& ConnectionAcceptorFixture::acceptor() { return acceptor_; }

const std::vector<std::uint8_t>& ConnectionAcceptorFixture::joinedPlayers()
    const {
  return joinedPlayers_;
}

}  // namespace rtype::server::testing
