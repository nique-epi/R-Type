#include "RouletteRulesFixtures.hpp"
#include <ostream>
#include "RouletteBets.hpp"

namespace rtype::server::testing {

void PrintTo(const PayoutCase& payoutCase, std::ostream* output) {
  *output << "kind " << static_cast<unsigned int>(payoutCase.bet.kind)
          << " from " << static_cast<unsigned int>(payoutCase.bet.firstNumber)
          << " for " << payoutCase.bet.amount << ", drawn "
          << static_cast<unsigned int>(payoutCase.drawn) << " pays "
          << payoutCase.payout;
}

}  // namespace rtype::server::testing
