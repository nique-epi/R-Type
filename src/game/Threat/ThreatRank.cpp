#include "ThreatRank.hpp"
#include <algorithm>
#include <cstddef>
#include "PercentConstants.hpp"
#include "TeamFirepower.hpp"
#include "ThreatConstants.hpp"
#include "ThreatLevel.hpp"

namespace rtype::game {

namespace {

/** @returns The ticks of THREAT_RISE_TICKS the threat @p team calls for
 * stands at: its share of the way from the gun of the sheet to the strongest
 * weapon, none for a team that fires no more than its gun. */
std::size_t calledTicksOf(const TeamFirepower& team) {
  const int way = team.highestPercent - WHOLE_PERCENT;
  if (way <= 0) {
    return 0;
  }
  const int gone = std::clamp(team.meanPercent - WHOLE_PERCENT, 0, way);
  return static_cast<std::size_t>(gone) * THREAT_RISE_TICKS /
         static_cast<std::size_t>(way);
}

}  // namespace

void ThreatRank::observe(const TeamFirepower& team) {
  risenTicks_ = std::min(risenTicks_ + 1, calledTicksOf(team));
}

ThreatLevel ThreatRank::level() const {
  const auto percent =
      static_cast<int>(risenTicks_ * static_cast<std::size_t>(WHOLE_PERCENT) /
                       THREAT_RISE_TICKS);
  return ThreatLevel{
      .percent = percent,
      .tier = std::min(HIGHEST_THREAT_TIER, percent / THREAT_PERCENT_PER_TIER)};
}

}  // namespace rtype::game
