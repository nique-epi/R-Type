#include "KopecksReward.hpp"
#include <algorithm>
#include <cstdint>
#include <span>
#include "FinalPilot.hpp"
#include "GameEnded.hpp"
#include "GameKopecks.hpp"

namespace rtype::client {

std::uint32_t kopecksEarnedBy(std::span<const FinalPilot> pilots,
                              network::WireGameOutcome outcome) {
  const auto own = std::ranges::find_if(pilots, &FinalPilot::isLocalPlayer);
  if (own == pilots.end()) {
    return 0;
  }
  const std::uint32_t earned = network::kopecksFor(own->score);
  return outcome == network::WireGameOutcome::Victory
             ? network::withVictoryBonus(earned)
             : earned;
}

}  // namespace rtype::client
