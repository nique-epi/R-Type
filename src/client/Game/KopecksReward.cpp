#include "KopecksReward.hpp"
#include <algorithm>
#include <cstdint>
#include <span>
#include "FinalPilot.hpp"
#include "GameGain.hpp"
#include "GameKopecks.hpp"

namespace rtype::client {

std::uint32_t kopecksEarnedBy(std::span<const FinalPilot> pilots) {
  const auto own = std::ranges::find_if(pilots, &FinalPilot::isLocalPlayer);
  if (own == pilots.end()) {
    return 0;
  }
  return network::kopecksFor(
      network::GameGain{.score = own->score, .bounty = own->bounty});
}

}  // namespace rtype::client
