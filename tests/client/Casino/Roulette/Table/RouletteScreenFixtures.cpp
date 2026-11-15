#include "RouletteScreenFixtures.hpp"
#include <cstdint>
#include "RouletteRound.hpp"
#include "RouletteScreenTestConstants.hpp"

namespace rtype::client::testing {

using network::RouletteRound;
using network::RouletteSeat;

RouletteRound bettingRound(std::uint16_t number, std::uint32_t aceStake) {
  return RouletteRound{
      .round = number,
      .phase = network::RoulettePhase::Betting,
      .ticksLeft = TWELVE_SECONDS_OF_TICKS,
      .winningNumber = 0,
      .seats = {RouletteSeat{.playerNumber = ACE_NUMBER, .stake = aceStake},
                RouletteSeat{.playerNumber = LUNA_NUMBER}}};
}

RouletteRound spinningRound() {
  RouletteRound round = bettingRound(A_ROUND, STAKE_OF_ACE);
  round.phase = network::RoulettePhase::Spinning;
  round.winningNumber = SEVENTEEN_WINS;
  return round;
}

RouletteRound resultRound(std::uint32_t acePayout) {
  RouletteRound round = spinningRound();
  round.phase = network::RoulettePhase::Result;
  round.seats.front().payout = acePayout;
  return round;
}

}  // namespace rtype::client::testing
