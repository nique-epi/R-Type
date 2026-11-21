#pragma once

#include "BehaviourRegistry.hpp"

namespace rtype::game {

/** @returns A registry holding every behaviour built into the game: the
 * paths (StraightBehaviour, WeaveBehaviour, ScatterBehaviour, AimedBehaviour)
 * and the attacks of the Bydos of the assets. */
[[nodiscard]] BehaviourRegistry builtInBehaviours();

}  // namespace rtype::game
