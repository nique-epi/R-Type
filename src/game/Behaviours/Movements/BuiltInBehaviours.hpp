#pragma once

#include "BehaviourRegistry.hpp"

namespace rtype::game {

/** @returns A registry holding every behaviour built into the game:
 * StraightBehaviour and WeaveBehaviour. */
[[nodiscard]] BehaviourRegistry builtInBehaviours();

}  // namespace rtype::game
