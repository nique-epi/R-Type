#include "HurtZoneSystemFixtures.hpp"
#include <vector>
#include "CollisionScene.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "HurtBox.hpp"
#include "HurtZone.hpp"
#include "HurtZoneSystem.hpp"
#include "ShipContact.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

std::vector<ShipContact> contactsOf(CollisionScene& scene) {
  HurtZoneSystem system(scene.events);
  std::vector<ShipContact> received;
  const auto handle = scene.events.subscribe<ShipContact>(
      [&received](const ShipContact& contact) { received.push_back(contact); });
  system.update(scene.components, engine::SIMULATION_TICK_DURATION);
  scene.events.dispatch();
  scene.events.unsubscribe(handle);
  return received;
}

engine::Entity enemyWith(CollisionScene& scene, float centerX,
                         const std::vector<HurtBox>& boxes) {
  const engine::Entity enemy =
      scene.spawnWithoutCollisionBox(EntityType::Enemy, centerX, 0.0F);
  scene.components.add(enemy, HurtZone{.boxes = boxes});
  return enemy;
}

}  // namespace rtype::game::testing
