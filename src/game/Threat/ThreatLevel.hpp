#pragma once

namespace rtype::game {

/**
 * @brief How much the enemies of a game answer the firepower of its team: the
 * threat in percent of the most, from 0 to WHOLE_PERCENT, and its tier, from
 * 0 to HIGHEST_THREAT_TIER, which the waves and the levels read to send more
 * enemies.
 */
struct ThreatLevel {
  int percent{0};
  int tier{0};

  bool operator==(const ThreatLevel&) const = default;
};

}  // namespace rtype::game
