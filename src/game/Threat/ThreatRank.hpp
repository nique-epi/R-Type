#pragma once

#include <cstddef>
#include "TeamFirepower.hpp"
#include "ThreatLevel.hpp"

namespace rtype::game {

/**
 * @brief The threat a team makes as its firepower grows, which the enemies
 * of its game answer (see ThreatLevel).
 *
 * The threat a firepower calls for is its share of the way from the gun of
 * the sheet to its strongest weapon: (mean - 100) / (highest - 100), between
 * 0 and 1. The threat rises toward it by one tick of THREAT_RISE_TICKS at a
 * time, so it takes THREAT_RISE to climb the whole way, and falls to it at
 * once: a ship that dies goes back to the gun of its sheet, and the enemies
 * made after it no longer answer its weapon.
 */
class ThreatRank {
 public:
  /** @brief Moves the threat one tick toward what @p team calls for. */
  void observe(const TeamFirepower& team);

  /** @returns The threat now, in percent rounded down, and its tier. */
  [[nodiscard]] ThreatLevel level() const;

 private:
  std::size_t risenTicks_{0};
};

}  // namespace rtype::game
