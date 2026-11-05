#include "RtypeSystems.hpp"
#include <memory>
#include "CollisionSystem.hpp"
#include "EventBus.hpp"
#include "HurtZoneSystem.hpp"
#include "MovementSystem.hpp"
#include "PilotSystem.hpp"
#include "PlayfieldExitSystem.hpp"
#include "RtypeSystemConstants.hpp"
#include "SystemScheduler.hpp"

namespace rtype::server {

void addRtypeSystems(engine::SystemScheduler& systems,
                     engine::EventBus& events) {
  systems.add(std::make_unique<game::PilotSystem>(events), PILOT_SYSTEM_NAME);
  systems.add(std::make_unique<game::MovementSystem>(), MOVEMENT_SYSTEM_NAME);
  systems.add(std::make_unique<game::PlayfieldExitSystem>(events),
              PLAYFIELD_EXIT_SYSTEM_NAME);
  systems.add(std::make_unique<game::CollisionSystem>(events),
              COLLISION_SYSTEM_NAME);
  systems.add(std::make_unique<game::HurtZoneSystem>(events),
              HURT_ZONE_SYSTEM_NAME);
}

}  // namespace rtype::server
