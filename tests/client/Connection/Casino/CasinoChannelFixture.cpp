#include "CasinoChannelFixture.hpp"
#include <cstdint>
#include <vector>
#include "CasinoChannel.hpp"
#include "CasinoChannelTestConstants.hpp"
#include "CasinoPlace.hpp"
#include "ConnectionAccepted.hpp"
#include "DatagramCodec.hpp"
#include "GameRequest.hpp"
#include "IncomingDatagram.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "RecordingWaitingRoom.hpp"
#include "RouletteBets.hpp"
#include "RouletteRound.hpp"
#include "ServerSessionFixtures.hpp"
#include "ServerSessionTestConstants.hpp"
#include "SessionEntityTypesFixtures.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

void CasinoChannelFixture::connectAndGetAccepted() {
  session_.connect(server(), NICKNAME, GameRequest{});
  network::PacketHeader header;
  header.sessionToken = SESSION_TOKEN;
  link_.deliver(network::IncomingDatagram{
      .sender = server(),
      .payload = network::DatagramCodec::encode(
          header,
          network::ConnectionAccepted{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                                      .content = localAnnouncement()})});
  update();
}

void CasinoChannelFixture::update(engine::Duration elapsed) {
  session_.update(elapsed);
}

void CasinoChannelFixture::serverSends(const network::RouletteRound& round) {
  link_.deliver(network::IncomingDatagram{
      .sender = server(),
      .payload =
          network::DatagramCodec::encode(sessionHeader(nextSequence_), round)});
  ++nextSequence_;
  update();
}

network::RouletteRound CasinoChannelFixture::roundListingThePlayer(
    std::uint16_t number, network::RoulettePhase phase) {
  const bool drawn = phase != network::RoulettePhase::Betting;
  return network::RouletteRound{
      .round = number,
      .phase = phase,
      .ticksLeft = 0,
      .winningNumber = drawn ? BET_NUMBER : std::uint8_t{0},
      .seats = {
          network::RouletteSeat{.playerNumber = ACCEPTED_PLAYER_NUMBER,
                                .stake = STAKE_OF_A_HUNDRED,
                                .payout = drawn ? PAYOUT_OF_A_HUNDRED : 0}}};
}

CasinoChannel& CasinoChannelFixture::casino() { return casino_; }

RecordingWaitingRoom& CasinoChannelFixture::room() { return room_; }

std::vector<network::CasinoPlace> CasinoChannelFixture::sentPlaces() const {
  std::vector<network::CasinoPlace> places;
  for (const network::OutgoingDatagram& datagram : link_.sent()) {
    if (holds<network::CasinoPlace>(datagram)) {
      places.push_back(onlyMessageOf<network::CasinoPlace>(datagram));
    }
  }
  return places;
}

std::vector<network::RouletteBets> CasinoChannelFixture::sentBets() const {
  std::vector<network::RouletteBets> bets;
  for (const network::OutgoingDatagram& datagram : link_.sent()) {
    if (holds<network::RouletteBets>(datagram)) {
      bets.push_back(onlyMessageOf<network::RouletteBets>(datagram));
    }
  }
  return bets;
}

}  // namespace rtype::client::testing
