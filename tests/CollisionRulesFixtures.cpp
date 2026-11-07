#include "CollisionRulesFixtures.hpp"
#include <gtest/gtest.h>
#include "Collision.hpp"
#include "CollisionRules.hpp"
#include "CollisionScene.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "MissileHit.hpp"
#include "ShipContact.hpp"

void record(CollisionScene& scene, RecordedCollisionEvents& recorded) {
  scene.events.subscribe<rtype::game::MissileHit>(
      [&recorded](const rtype::game::MissileHit& hit) {
        recorded.missileHits.push_back(hit);
      });
  scene.events.subscribe<rtype::game::ShipContact>(
      [&recorded](const rtype::game::ShipContact& contact) {
        recorded.shipContacts.push_back(contact);
      });
}

void expectNoEventFor(rtype::game::EntityType firstType,
                      rtype::game::EntityType secondType) {
  CollisionScene scene;
  const rtype::engine::Entity first = scene.spawn(firstType, 0.0F, 0.0F);
  const rtype::engine::Entity second = scene.spawn(secondType, 0.0F, 0.0F);
  const rtype::game::CollisionRules rules(scene.events, scene.components);
  RecordedCollisionEvents recorded;
  record(scene, recorded);

  scene.events.publish(
      rtype::engine::Collision{.first = first, .second = second});
  scene.events.dispatch();

  EXPECT_TRUE(recorded.missileHits.empty());
  EXPECT_TRUE(recorded.shipContacts.empty());
}
