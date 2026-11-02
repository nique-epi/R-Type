#include "RtypeSystems.hpp"
#include <memory>
#include "BydoControlSystem.hpp"
#include "CollisionSystem.hpp"
#include "EventBus.hpp"
#include "MovementSystem.hpp"
#include "PatternSystem.hpp"
#include "PlayfieldExitSystem.hpp"
#include "RtypeSystemConstants.hpp"
#include "SystemScheduler.hpp"

namespace rtype::server {

void addRtypeSystems(engine::SystemScheduler& systems,
                     engine::EventBus& events) {
  systems.add(std::make_unique<game::BydoControlSystem>(),
              BYDO_CONTROL_SYSTEM_NAME);
  systems.add(std::make_unique<game::PatternSystem>(), PATTERN_SYSTEM_NAME);
  systems.add(std::make_unique<game::MovementSystem>(), MOVEMENT_SYSTEM_NAME);
  systems.add(std::make_unique<game::PlayfieldExitSystem>(events),
              PLAYFIELD_EXIT_SYSTEM_NAME);
  systems.add(std::make_unique<game::CollisionSystem>(events),
              COLLISION_SYSTEM_NAME);
}

}  // namespace rtype::server
