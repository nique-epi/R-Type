#pragma once

#include "BehaviourView.hpp"

namespace rtype::game {

/**
 * @returns The vertical share that moves the body of @p view to @p height
 * during this tick, 0 when it cannot move during it (no top speed, or a step
 * of no time). A behaviour that follows a path computed from the age of its
 * body writes it, so the path does not depend on how the time is cut into
 * steps.
 */
[[nodiscard]] float verticalShareTowards(float height,
                                         const BehaviourView& view);

}  // namespace rtype::game
