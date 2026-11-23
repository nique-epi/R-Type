#pragma once

#include "TeamFirepower.hpp"
#include "ThreatLevel.hpp"

namespace rtype::server {

/**
 * @brief The threat of a game, which the module threat provides: the rules
 * tell it what the team fires at each tick of a game in progress, and read
 * how much the enemies they make answer it (see game::ThreatRank).
 *
 * A game built without the module has none: its enemies answer no weapon.
 */
class IThreatLevel {
 public:
  virtual ~IThreatLevel() = default;

  /** @brief Moves the threat one tick toward what @p team fires. */
  virtual void observe(const game::TeamFirepower& team) = 0;

  /** @returns The threat now. */
  [[nodiscard]] virtual game::ThreatLevel level() const = 0;
};

}  // namespace rtype::server
