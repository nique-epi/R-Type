#pragma once

#include "BehaviourRegistry.hpp"

namespace rtype::game {

/** @returns A registry holding every behaviour of R-Type: StraightBehaviour and
 * WeaveBehaviour. */
[[nodiscard]] BehaviourRegistry rtypeBehaviours();

}  // namespace rtype::game
