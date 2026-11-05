#include "ConnectionAcceptorFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "ConnectionAcceptorParts.hpp"
#include "ConnectionChallenge.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "SentDatagrams.hpp"
#include "ServerDatagrams.hpp"
#include "TimeConstants.hpp"

namespace rtype::server::testing {

ConnectionAcceptorFixture::ConnectionAcceptorFixture()
    : acceptor_(ConnectionAcceptorParts{
          .roster = roster_,
          .send =
              [this](network::OutgoingDatagram datagram) {
                answers_.push_back(std::move(datagram));
              },
          .admit =
              [this](const network::Endpoint& player,
                     std::uint64_t sessionToken) {
                admitted_.emplace_back(player, sessionToken);
              },
          .onPlayerJoined =
              [this](ConnectedPlayer& player) {
                joinedPlayers_.push_back(player.playerNumber);
              }}) {}

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
  return onlyMessageIn<network::ConnectionChallenge>(lastAnswer()).cookie;
}

void ConnectionAcceptorFixture::connect(const network::Endpoint& sender,
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

std::vector<std::string> ConnectionAcceptorFixture::journalLinesHolding(
    std::string_view text) const {
  return journal_.linesHolding(text);
}

}  // namespace rtype::server::testing
