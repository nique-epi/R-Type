#include "RouletteMemoryFixtures.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "BetsReading.hpp"
#include "BetsReceipt.hpp"
#include "CasinoRefusal.hpp"
#include "CasinoRoomState.hpp"
#include "RoomRound.hpp"
#include "RouletteBet.hpp"
#include "RouletteMemoryTestConstants.hpp"
#include "RoulettePhase.hpp"

namespace rtype::client::testing {

network::CasinoRoomState roomAtRound(std::uint64_t round) {
  return network::CasinoRoomState{.id = MEMORY_ROOM,
                                  .name = std::string{},
                                  .seats = 0,
                                  .round = round,
                                  .phase = network::RoulettePhase::Betting,
                                  .closesAt = {},
                                  .resultAt = {},
                                  .endsAt = {},
                                  .serverTime = {},
                                  .winningNumber = std::nullopt,
                                  .history = {},
                                  .players = {}};
}

BetsReading takenBets(std::uint64_t ask, std::uint64_t round,
                      const std::vector<network::RouletteBet>& bets) {
  std::uint32_t stake = 0;
  for (const network::RouletteBet& bet : bets) {
    stake += bet.amount;
  }
  return BetsReading{
      .ask = ask,
      .round = network::RoomRound{.room = MEMORY_ROOM, .round = round},
      .receipt = network::BetsReceipt{.round = round,
                                      .bets = bets,
                                      .stake = stake,
                                      .chips = CHIPS_LEFT},
      .refusal = std::nullopt};
}

BetsReading refusedBets(std::uint64_t ask, std::uint64_t round) {
  return BetsReading{
      .ask = ask,
      .round = network::RoomRound{.room = MEMORY_ROOM, .round = round},
      .receipt = std::nullopt,
      .refusal = CasinoRefusal::NotEnoughChips};
}

}  // namespace rtype::client::testing
