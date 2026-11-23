#pragma once

#include "PercentConstants.hpp"

namespace rtype::game {

/**
 * @brief What the ships of a team fire, in percent of the gun of their sheet
 * (see firepowerPercentOf()): the mean over the ships in the world, and the
 * most a weapon of their sheets gives. A team with no ship in the world fires
 * as the gun of its sheet.
 */
struct TeamFirepower {
  int meanPercent{WHOLE_PERCENT};
  int highestPercent{WHOLE_PERCENT};

  bool operator==(const TeamFirepower&) const = default;
};

}  // namespace rtype::game
