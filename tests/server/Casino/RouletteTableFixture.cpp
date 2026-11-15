#include "RouletteTableFixture.hpp"
#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>
#include "CasinoPlace.hpp"
#include "ConnectedPlayer.hpp"
#include "RouletteBets.hpp"
#include "RouletteRound.hpp"
#include "RouletteTable.hpp"
#include "RouletteTableTestConstants.hpp"
#include "ScriptedRouletteDraw.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

RouletteTableFixture::RouletteTableFixture()
    : draw_({SCRIPTED_NUMBER}), table_(roster_, draw_) {}

ConnectedPlayer& RouletteTableFixture::join(std::size_t index) {
  return *roster_.add(playerEndpoint(index),
                      std::string{NICKNAME_PREFIX} + std::to_string(index),
                      SESSION_TOKEN + index);
}

void RouletteTableFixture::sit(std::size_t index) {
  table_.onPlace(player(index), network::TablePlace::AtTable);
}

void RouletteTableFixture::standUp(std::size_t index) {
  table_.onPlace(player(index), network::TablePlace::InRoom);
}

void RouletteTableFixture::bet(std::size_t index,
                               const std::vector<network::RouletteBet>& bets,
                               bool spinVote) {
  table_.onBets(
      player(index),
      network::RouletteBets{
          .round = table_.round().round, .spinVote = spinVote, .bets = bets});
}

void RouletteTableFixture::leave(std::size_t index) {
  const std::optional<ConnectedPlayer> departed =
      roster_.remove(playerEndpoint(index));
  if (departed.has_value()) {
    table_.onPlayerClosing(*departed);
  }
}

void RouletteTableFixture::runTicks(std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    table_.onTick();
  }
}

std::optional<network::RouletteSeat> RouletteTableFixture::listedSeat(
    std::size_t index) {
  const network::RouletteRound round = table_.round();
  const auto found = std::ranges::find(round.seats, player(index).playerNumber,
                                       &network::RouletteSeat::playerNumber);
  if (found == round.seats.end()) {
    return std::nullopt;
  }
  return *found;
}

ConnectedPlayer& RouletteTableFixture::player(std::size_t index) {
  return *roster_.find(playerEndpoint(index));
}

RouletteTable& RouletteTableFixture::table() { return table_; }

const ScriptedRouletteDraw& RouletteTableFixture::draw() const { return draw_; }

}  // namespace rtype::server::testing
