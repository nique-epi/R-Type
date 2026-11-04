#pragma once

#include "EventBus.hpp"
#include "SystemScheduler.hpp"

namespace rtype::server {

/**
 * @brief Adds the systems that run on the world of an R-Type game each tick to
 * @p systems, in the order they run: BydoControlSystem, PatternSystem,
 * MovementSystem, PlayfieldExitSystem, HurtZoneSystem, then CollisionSystem.
 * The last three publish on @p events, which must outlive @p systems.
 */
void addRtypeSystems(engine::SystemScheduler& systems,
                     engine::EventBus& events);

}  // namespace rtype::server
