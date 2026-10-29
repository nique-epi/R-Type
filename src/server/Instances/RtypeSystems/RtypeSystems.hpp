#pragma once

#include "EventBus.hpp"
#include "SystemScheduler.hpp"

namespace rtype::server {

/**
 * @brief Adds the systems that run on the world of an R-Type game each tick to
 * @p systems, in the order they run: PatternSystem, MovementSystem,
 * PlayfieldExitSystem, then CollisionSystem. The last two publish on @p events,
 * which must outlive @p systems.
 */
void addRtypeSystems(engine::SystemScheduler& systems,
                     engine::EventBus& events);

}  // namespace rtype::server
