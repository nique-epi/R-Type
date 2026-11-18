#pragma once

#include "IHitHandler.hpp"

namespace rtype::game {

/** @brief The handler the sheet of a body names in `on_hit`, which the
 * BehaviourRegistry the sheets were read with owns, as a Pilot keeps its
 * behaviour. */
struct HitReaction {
  IHitHandler* handler{nullptr};
};

}  // namespace rtype::game
