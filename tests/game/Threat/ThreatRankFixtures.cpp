#include "ThreatRankFixtures.hpp"
#include <cstddef>
#include "TeamFirepower.hpp"
#include "ThreatLevel.hpp"
#include "ThreatRank.hpp"

namespace rtype::game::testing {

ThreatLevel levelAfter(ThreatRank& rank, const TeamFirepower& team,
                       std::size_t ticks) {
  for (std::size_t tick = 0; tick < ticks; ++tick) {
    rank.observe(team);
  }
  return rank.level();
}

}  // namespace rtype::game::testing
