#include "BanningAcceptorFixture.hpp"
#include <gtest/gtest.h>
#include <cstdint>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "Ban.hpp"
#include "BanList.hpp"
#include "ConnectionAcceptor.hpp"
#include "ConnectionAcceptorFixtures.hpp"
#include "ConnectionAcceptorParts.hpp"
#include "Endpoint.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"
#include "ServerDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

BanningAcceptorFixture::BanningAcceptorFixture()
    : acceptor_(ConnectionAcceptorParts{
          .roster = roster_,
          .send =
              [this](network::OutgoingDatagram datagram) {
                answers_.push_back(std::move(datagram));
              },
          .admit = []([[maybe_unused]] const network::Endpoint& player,
                      [[maybe_unused]] std::uint64_t sessionToken) {},
          .onPlayerJoined = {},
          .content = gameContent(),
          .bans = &bans_}) {}

void BanningAcceptorFixture::publish(std::vector<Ban> bans) {
  bans_.publish(BanList{std::move(bans)});
}

void BanningAcceptorFixture::request(const network::Endpoint& sender,
                                     const std::string& nickname) {
  const network::IncomingDatagram received{
      .sender = sender, .payload = requestDatagramOf(nickname, 0)};
  acceptor_.receive(std::span{&received, 1}, REQUEST_TIME);
}

const network::OutgoingDatagram& BanningAcceptorFixture::lastAnswer() const {
  static const network::OutgoingDatagram none{};
  if (answers_.empty()) {
    ADD_FAILURE() << "the acceptor answered nothing";
    return none;
  }
  return answers_.back();
}

}  // namespace rtype::server::testing
