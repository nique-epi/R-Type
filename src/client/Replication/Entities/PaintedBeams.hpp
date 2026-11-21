#pragma once

#include <vector>
#include "AnnouncedAction.hpp"
#include "CurrentAction.hpp"
#include "HurtZone.hpp"

namespace rtype::client {

/**
 * @brief Component of an enemy whose sheet asks the client to paint its hurt
 * beams (`[appearance.hurt_beams]`): its hurt zone, the actions announced and
 * not shown yet, and the action shown at the tick the client draws, with how
 * long it has been shown (see showPaintedBeams()).
 */
struct PaintedBeams {
  game::HurtZone zone;
  std::vector<AnnouncedAction> announced;
  game::CurrentAction shown{};
  /** @brief The server tick of the action shown, which its time is counted
   * from at each frame. */
  double shownTick{0.0};
};

}  // namespace rtype::client
