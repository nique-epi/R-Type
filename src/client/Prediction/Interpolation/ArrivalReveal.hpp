#pragma once

#include "ComponentRegistry.hpp"
#include "EntityFeedback.hpp"
#include "InterpolationClock.hpp"

namespace rtype::client {

/**
 * @brief Shows every entity awaiting its arrival whose first state the tick
 * @p clock shows has reached: gives it back its sprite and plays its
 * appearance, its shot and its muzzle flash. A missile then leaves the muzzle
 * of the shooter as the client shows it and moves from its first frame,
 * instead of standing still where the server created it until the tick shown
 * catches up. Call it after placeInterpolatedPositions(), so the appearance
 * plays where the entity is shown. Not during a forEach of the registry.
 */
void revealArrivedEntities(const InterpolationClock& clock,
                           engine::ComponentRegistry& components,
                           const EntityFeedback& feedback);

}  // namespace rtype::client
