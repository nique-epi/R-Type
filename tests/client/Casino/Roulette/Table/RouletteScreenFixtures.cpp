#include "RouletteScreenFixtures.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "BetsReading.hpp"
#include "BetsReceipt.hpp"
#include "CasinoPlayer.hpp"
#include "CasinoRefusal.hpp"
#include "CasinoRoomState.hpp"
#include "MillisecondDate.hpp"
#include "RoomRound.hpp"
#include "RouletteBet.hpp"
#include "RoulettePhase.hpp"
#include "RouletteScreenTestConstants.hpp"
#include "RoundBets.hpp"

namespace rtype::client::testing {

namespace {

using network::MillisecondDate;

/** @returns ACE with @p stake on the cloth that paid @p payout, then LUNA,
 * seated in the room. */
std::vector<network::CasinoPlayer> playersOfTheRoom(
    std::uint32_t stake, std::optional<std::uint32_t> payout) {
  return {network::CasinoPlayer{.nickname = std::string{ACE_NAME},
                                .stake = stake,
                                .payout = payout,
                                .bets = {}},
          network::CasinoPlayer{.nickname = std::string{LUNA_NAME},
                                .stake = 0,
                                .payout = payout.has_value()
                                              ? std::optional<std::uint32_t>(0)
                                              : std::nullopt,
                                .bets = {}}};
}

}  // namespace

network::CasinoRoomState bettingRoom(std::uint64_t round) {
  const MillisecondDate opened{MASTER_TIME_AS_THE_BETS_OPEN};
  return network::CasinoRoomState{
      .id = NEBULA_LOUNGE,
      .name = std::string{NEBULA_LOUNGE_NAME},
      .seats = NEBULA_LOUNGE_SEATS,
      .round = round,
      .phase = network::RoulettePhase::Betting,
      .closesAt = opened + BETTING_TIME,
      .resultAt = opened + BETTING_TIME + SPINNING_TIME,
      .endsAt = opened + BETTING_TIME + SPINNING_TIME + RESULT_TIME,
      .serverTime = opened,
      .winningNumber = std::nullopt,
      .history = {},
      .players = playersOfTheRoom(0, std::nullopt)};
}

network::CasinoRoomState spinningRoom() {
  network::CasinoRoomState room = bettingRoom(A_ROUND);
  room.phase = network::RoulettePhase::Spinning;
  room.serverTime = room.closesAt + A_SECOND_INTO_THE_PHASE;
  room.winningNumber = SEVENTEEN_WINS;
  room.players = playersOfTheRoom(STAKE_OF_ACE, std::nullopt);
  return room;
}

network::CasinoRoomState drawnSpinningRoom(std::uint32_t acePayout) {
  network::CasinoRoomState room = spinningRoom();
  room.players = playersOfTheRoom(STAKE_OF_ACE, acePayout);
  return room;
}

network::CasinoRoomState resultRoom(std::uint32_t acePayout) {
  network::CasinoRoomState room = drawnSpinningRoom(acePayout);
  room.phase = network::RoulettePhase::Result;
  room.serverTime = room.resultAt + A_SECOND_INTO_THE_PHASE;
  return room;
}

network::RoundBets betsOfAce(std::uint32_t payout) {
  return network::RoundBets{.round = A_ROUND,
                            .bets = {TWENTY_FIVE_ON_17},
                            .stake = STAKE_OF_ACE,
                            .payout = payout,
                            .winningNumber = SEVENTEEN_WINS};
}

BetsReading betsTaken(const std::vector<network::RouletteBet>& bets) {
  std::uint32_t stake = 0;
  for (const network::RouletteBet& bet : bets) {
    stake += bet.amount;
  }
  return BetsReading{
      .ask = FIRST_BETS_ANSWER,
      .round = network::RoomRound{.room = NEBULA_LOUNGE, .round = A_ROUND},
      .receipt = network::BetsReceipt{.round = A_ROUND,
                                      .bets = bets,
                                      .stake = stake,
                                      .chips = CHIPS_LEFT_TO_ACE},
      .refusal = std::nullopt};
}

BetsReading betsRefused(CasinoRefusal refusal) {
  return BetsReading{
      .ask = FIRST_BETS_ANSWER,
      .round = network::RoomRound{.room = NEBULA_LOUNGE, .round = A_ROUND},
      .receipt = std::nullopt,
      .refusal = refusal};
}

}  // namespace rtype::client::testing
