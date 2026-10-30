#include "RtypeSystems.hpp"
#include <memory>
#include "BydoControlSystem.hpp"
#include "CollisionSystem.hpp"
#include "EventBus.hpp"
#include "MovementSystem.hpp"
#include "PatternSystem.hpp"
#include "PlayfieldExitSystem.hpp"
#include "SystemScheduler.hpp"

namespace rtype::server {

void addRtypeSystems(engine::SystemScheduler& systems,
                     engine::EventBus& events) {
  systems.add(std::make_unique<game::BydoControlSystem>());
  systems.add(std::make_unique<game::PatternSystem>());
  systems.add(std::make_unique<game::MovementSystem>());
  systems.add(std::make_unique<game::PlayfieldExitSystem>(events));
  systems.add(std::make_unique<game::CollisionSystem>(events));
}

}  // namespace rtype::server
