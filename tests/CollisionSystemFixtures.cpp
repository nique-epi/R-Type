#include "CollisionSystemFixtures.hpp"
#include <vector>
#include "Collision.hpp"
#include "CollisionScene.hpp"
#include "CollisionSystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

std::vector<engine::Collision> updateAndCollect(CollisionScene& scene,
                                                CollisionSystem& system) {
  std::vector<engine::Collision> received;
  const auto handle = scene.events.subscribe<engine::Collision>(
      [&received](const engine::Collision& collision) {
        received.push_back(collision);
      });
  system.update(scene.components, engine::SIMULATION_TICK_DURATION);
  scene.events.dispatch();
  scene.events.unsubscribe(handle);
  return received;
}

}  // namespace rtype::game::testing
