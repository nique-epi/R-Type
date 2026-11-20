#include "CasinoMessageTextFixtures.hpp"
#include <cstddef>
#include <nlohmann/json.hpp>
#include <string>
#include "BetsReceipt.hpp"
#include "CasinoMessageTextTestConstants.hpp"
#include "CasinoPlayer.hpp"
#include "CasinoRoom.hpp"
#include "CasinoRoomListing.hpp"
#include "CasinoRoomState.hpp"
#include "MasterMessageConstants.hpp"
#include "MasterMessageText.hpp"
#include "MillisecondDate.hpp"
#include "RouletteBet.hpp"
#include "RouletteBetKind.hpp"
#include "RouletteBets.hpp"
#include "RoulettePhase.hpp"
#include "RoundBets.hpp"

namespace rtype::network::testing {

RouletteBets twoBets() {
  return RouletteBets{.bets = {RouletteBet{.kind = RouletteBetKind::Straight,
                                           .firstNumber = STRAIGHT_NUMBER,
                                           .amount = STRAIGHT_AMOUNT},
                               RouletteBet{.kind = RouletteBetKind::Red,
                                           .firstNumber = 0,
                                           .amount = RED_AMOUNT}}};
}

CasinoRoomListing oneRoomListing() {
  return CasinoRoomListing{
      .rooms = {CasinoRoom{.id = TEST_ROOM_ID,
                           .name = std::string{TEST_ROOM_NAME},
                           .seats = TEST_ROOM_SEATS,
                           .seated = TEST_ROOM_SEATED,
                           .round = TEST_ROUND,
                           .phase = RoulettePhase::Betting}}};
}

CasinoRoomState drawnRoomState() {
  return CasinoRoomState{
      .id = TEST_ROOM_ID,
      .name = std::string{TEST_ROOM_NAME},
      .seats = TEST_ROOM_SEATS,
      .round = TEST_ROUND,
      .phase = RoulettePhase::Spinning,
      .closesAt = MillisecondDate{TEST_CLOSES_AT},
      .resultAt = MillisecondDate{TEST_RESULT_AT},
      .endsAt = MillisecondDate{TEST_ENDS_AT},
      .serverTime = MillisecondDate{TEST_SERVER_TIME},
      .winningNumber = TEST_WINNING_NUMBER,
      .history = {TEST_EARLIER_NUMBER, TEST_EARLIEST_NUMBER},
      .players = {CasinoPlayer{.nickname = std::string{TEST_CASINO_NICKNAME},
                               .stake = TEST_STAKE,
                               .payout = TEST_PAYOUT,
                               .bets = twoBets().bets}}};
}

std::string roomStateTextWithoutBets() {
  nlohmann::json document =
      nlohmann::json::parse(casinoRoomStateText(drawnRoomState()));
  for (nlohmann::json& player : document.at(PLAYERS_MEMBER)) {
    player.erase(BETS_MEMBER);
  }
  return document.dump();
}

BetsReceipt twoBetsReceipt() {
  return BetsReceipt{.round = TEST_ROUND,
                     .bets = twoBets().bets,
                     .stake = TEST_STAKE,
                     .chips = TEST_CHIPS_LEFT};
}

RoundBets paidRoundBets() {
  return RoundBets{.round = TEST_ROUND,
                   .bets = twoBets().bets,
                   .stake = TEST_STAKE,
                   .payout = TEST_PAYOUT,
                   .winningNumber = TEST_WINNING_NUMBER};
}

std::string straightBetsBody(std::size_t count) {
  RouletteBets bets;
  for (std::size_t index = 0; index < count; ++index) {
    bets.bets.push_back(RouletteBet{.kind = RouletteBetKind::Straight,
                                    .firstNumber = 1,
                                    .amount = STRAIGHT_AMOUNT});
  }
  return rouletteBetsText(bets);
}

}  // namespace rtype::network::testing
