#include "ServerLeaderboardFixture.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "ConnectionAccepted.hpp"
#include "DatagramCodec.hpp"
#include "IncomingDatagram.hpp"
#include "LeaderboardAnswer.hpp"
#include "LeaderboardRequest.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "ServerLeaderboard.hpp"
#include "ServerLeaderboardTestConstants.hpp"
#include "ServerSession.hpp"
#include "ServerSessionFixtures.hpp"
#include "ServerSessionTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

network::LeaderboardRequest ServerLeaderboardFixture::askedRequest() {
  return network::LeaderboardRequest{
      .playerCount = ASKED_PILOTS, .difficulty = std::string{ASKED_DIFFICULTY}};
}

network::LeaderboardAnswer ServerLeaderboardFixture::answerFor(
    std::string_view difficulty) {
  return network::LeaderboardAnswer{
      .asked =
          network::LeaderboardRequest{.playerCount = ASKED_PILOTS,
                                      .difficulty = std::string{difficulty}},
      .rows = {network::LeaderboardRow{.rank = 1,
                                       .score = 1000,
                                       .playedAt = 0,
                                       .nickname = std::string{TOP_NICKNAME}}},
      .ownRow = std::nullopt};
}

void ServerLeaderboardFixture::connectAndGetAccepted() {
  session_.connect(server(), NICKNAME);
  network::PacketHeader header;
  header.sessionToken = SESSION_TOKEN;
  link_.deliver(network::IncomingDatagram{
      .sender = server(),
      .payload = network::DatagramCodec::encode(
          header, network::ConnectionAccepted{.playerNumber =
                                                  ACCEPTED_PLAYER_NUMBER})});
  update();
}

void ServerLeaderboardFixture::update(engine::Duration elapsed) {
  session_.update(elapsed);
}

void ServerLeaderboardFixture::deliverInSession(
    const network::LeaderboardAnswer& answer, std::uint16_t sequence) {
  network::PacketHeader header;
  header.sessionToken = SESSION_TOKEN;
  header.sequence = sequence;
  link_.deliver(network::IncomingDatagram{
      .sender = server(),
      .payload = network::DatagramCodec::encode(header, answer)});
}

ServerLeaderboard& ServerLeaderboardFixture::leaderboard() {
  return leaderboard_;
}

ServerSession& ServerLeaderboardFixture::session() { return session_; }

const std::vector<network::OutgoingDatagram>& ServerLeaderboardFixture::sent()
    const {
  return link_.sent();
}

std::size_t ServerLeaderboardFixture::sentRequestCount() const {
  return static_cast<std::size_t>(
      std::ranges::count_if(link_.sent(), holds<network::LeaderboardRequest>));
}

}  // namespace rtype::client::testing
