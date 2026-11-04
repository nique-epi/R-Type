#pragma once

#include "ComponentRegistry.hpp"
#include "InterpolationClock.hpp"

namespace rtype::client {

/**
 * @brief Puts every entity that has a PositionHistory at the position it has
 * at the tick @p clock shows, so entities move smoothly between the states
 * and not at the pace of the states. Call it after the messages were applied
 * and before the position of the own ship is predicted. Not during a forEach
 * of the registry.
 */
void placeInterpolatedPositions(const InterpolationClock& clock,
                                engine::ComponentRegistry& components);

}  // namespace rtype::client
