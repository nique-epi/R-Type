#include "RouletteRoundAnnouncerFixture.hpp"
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include "OutgoingDatagram.hpp"
#include "RouletteRound.hpp"
#include "RouletteRoundAnnouncer.hpp"
#include "RouletteRoundAnnouncerTestConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

RouletteRoundAnnouncerFixture::RouletteRoundAnnouncerFixture()
    : announcer_(roster_, [this](network::OutgoingDatagram datagram) {
        sent_.push_back(std::move(datagram));
      }) {}

void RouletteRoundAnnouncerFixture::join(std::size_t index) {
  static_cast<void>(
      roster_.add(playerEndpoint(index),
                  std::string{NICKNAME_PREFIX} + std::to_string(index),
                  SESSION_TOKEN + index));
}

void RouletteRoundAnnouncerFixture::announce(
    const network::RouletteRound& round, std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    announcer_.onTick(round);
  }
}

std::vector<network::RouletteRound> RouletteRoundAnnouncerFixture::roundsTo(
    std::size_t index) const {
  std::vector<network::RouletteRound> rounds;
  for (const network::OutgoingDatagram& datagram :
       sentTo(sent_, playerEndpoint(index))) {
    for (const network::RouletteRound& round :
         messagesIn<network::RouletteRound>(datagram)) {
      rounds.push_back(round);
    }
  }
  return rounds;
}

RouletteRoundAnnouncer& RouletteRoundAnnouncerFixture::announcer() {
  return announcer_;
}

network::RouletteRound RouletteRoundAnnouncerFixture::roundListing(
    std::size_t index) {
  return network::RouletteRound{
      .round = 0,
      .phase = network::RoulettePhase::Betting,
      .ticksLeft = SOME_TICKS_LEFT,
      .winningNumber = 0,
      .seats = {network::RouletteSeat{
          .playerNumber = roster_.find(playerEndpoint(index))->playerNumber,
          .stake = 0,
          .payout = 0}}};
}

}  // namespace rtype::server::testing
