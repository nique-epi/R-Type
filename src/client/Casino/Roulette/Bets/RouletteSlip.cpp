#include "RouletteSlip.hpp"
#include <algorithm>
#include <cstdint>
#include <vector>
#include "RouletteBet.hpp"
#include "RouletteConstants.hpp"

namespace rtype::client {

namespace {

/** @returns The chips of @p bets added up, wide enough never to wrap. */
std::uint64_t stakeOf(const std::vector<network::RouletteBet>& bets) {
  std::uint64_t stake = 0;
  for (const network::RouletteBet& bet : bets) {
    stake += bet.amount;
  }
  return stake;
}

/** @returns Whether @p bets may lie on the cloth for a player holding
 * @p chipsHeld chips. */
bool fits(const std::vector<network::RouletteBet>& bets,
          std::uint32_t chipsHeld) {
  const std::uint64_t stake = stakeOf(bets);
  return bets.size() <= network::MAXIMUM_ROULETTE_BETS &&
         stake <= network::MAXIMUM_ROUND_STAKE && stake <= chipsHeld;
}

/** @returns Whether @p laid lies on the same place as @p bet. */
bool samePlace(const network::RouletteBet& laid,
               const network::RouletteBet& bet) {
  return laid.kind == bet.kind && laid.firstNumber == bet.firstNumber;
}

}  // namespace

const std::vector<network::RouletteBet>& RouletteSlip::bets() const {
  return bets_;
}

std::uint32_t RouletteSlip::stake() const {
  return static_cast<std::uint32_t>(stakeOf(bets_));
}

bool RouletteSlip::lay(const network::RouletteBet& bet,
                       std::uint32_t chipsHeld) {
  std::vector<network::RouletteBet> laid = bets_;
  const auto stack = std::ranges::find_if(
      laid, [&bet](const network::RouletteBet& held) noexcept {
        return samePlace(held, bet);
      });
  if (stack == laid.end()) {
    laid.push_back(bet);
  } else {
    stack->amount += bet.amount;
  }
  return replaceWith(laid, chipsHeld);
}

void RouletteSlip::clear() { bets_.clear(); }

bool RouletteSlip::replaceWith(const std::vector<network::RouletteBet>& bets,
                               std::uint32_t chipsHeld) {
  if (!fits(bets, chipsHeld)) {
    return false;
  }
  bets_ = bets;
  return true;
}

}  // namespace rtype::client
