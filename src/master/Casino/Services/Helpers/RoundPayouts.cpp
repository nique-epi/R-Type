#include "RoundPayouts.hpp"
#include <cstdint>
#include <map>
#include <vector>
#include "PlayerBet.hpp"
#include "RouletteBet.hpp"
#include "RouletteRules.hpp"

namespace rtype::master {

std::uint32_t stakeOf(const std::vector<network::RouletteBet>& bets) {
  std::uint32_t stake = 0;
  for (const network::RouletteBet& bet : bets) {
    stake += bet.amount;
  }
  return stake;
}

std::map<std::int64_t, std::uint32_t> stakesOf(
    const std::vector<PlayerBet>& bets) {
  std::map<std::int64_t, std::uint32_t> stakes;
  for (const PlayerBet& placed : bets) {
    stakes[placed.userId] += placed.bet.amount;
  }
  return stakes;
}

std::map<std::int64_t, std::uint32_t> payoutsOf(
    const std::vector<PlayerBet>& bets, std::uint8_t drawn) {
  std::map<std::int64_t, std::uint32_t> payouts;
  for (const PlayerBet& placed : bets) {
    payouts[placed.userId] +=
        static_cast<std::uint32_t>(network::payoutOf(placed.bet, drawn));
  }
  return payouts;
}

std::uint32_t payoutOfBets(const std::vector<network::RouletteBet>& bets,
                           std::uint8_t drawn) {
  std::uint32_t payout = 0;
  for (const network::RouletteBet& bet : bets) {
    payout += static_cast<std::uint32_t>(network::payoutOf(bet, drawn));
  }
  return payout;
}

}  // namespace rtype::master
