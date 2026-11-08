#include "CollisionRulesFixtures.hpp"
#include <gtest/gtest.h>
#include <cstdint>
#include "Collision.hpp"
#include "CollisionRules.hpp"
#include "CollisionScene.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "MissileHit.hpp"
#include "MissileOwner.hpp"
#include "ShipContact.hpp"
#include "ShipPilot.hpp"

namespace rtype::game::testing {

void record(CollisionScene& scene, Recorded& recorded) {
  scene.events.subscribe<MissileHit>([&recorded](const MissileHit& hit) {
    recorded.missileHits.push_back(hit);
  });
  scene.events.subscribe<ShipContact>([&recorded](const ShipContact& contact) {
    recorded.shipContacts.push_back(contact);
  });
}

engine::Entity spawnMissileOf(CollisionScene& scene, std::uint8_t owner) {
  const engine::Entity missile =
      scene.spawn(EntityType::PlayerMissile, 0.0F, 0.0F);
  scene.components.add(missile, MissileOwner{.playerNumber = owner});
  return missile;
}

engine::Entity spawnShipFlownBy(CollisionScene& scene, std::uint8_t pilot) {
  const engine::Entity ship = scene.spawn(EntityType::Player, 0.0F, 0.0F);
  scene.components.add(ship, ShipPilot{.playerNumber = pilot});
  return ship;
}

void expectNoEventFor(EntityType firstType, EntityType secondType) {
  CollisionScene scene;
  const engine::Entity first = scene.spawn(firstType, 0.0F, 0.0F);
  const engine::Entity second = scene.spawn(secondType, 0.0F, 0.0F);
  const CollisionRules rules(scene.events, scene.components);
  Recorded recorded;
  record(scene, recorded);

  scene.events.publish(engine::Collision{.first = first, .second = second});
  scene.events.dispatch();

  EXPECT_TRUE(recorded.missileHits.empty());
  EXPECT_TRUE(recorded.shipContacts.empty());
}

engine::Collision CollisionRulesInAnyOrder::collisionOf(engine::Entity entity,
                                                        engine::Entity other) {
  return GetParam() ? engine::Collision{.first = other, .second = entity}
                    : engine::Collision{.first = entity, .second = other};
}

}  // namespace rtype::game::testing
