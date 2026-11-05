#include "DamageScene.hpp"
#include <cstdint>
#include "EnemyKilled.hpp"
#include "Entity.hpp"
#include "EntityDamage.hpp"
#include "EntityDestruction.hpp"
#include "EntityType.hpp"
#include "Health.hpp"
#include "MissileHit.hpp"
#include "MissileOwner.hpp"
#include "NetworkIdentity.hpp"
#include "ScoreValue.hpp"

namespace rtype::game::testing {

DamageScene::DamageScene() {
  scene.events.subscribe<EntityDamage>(
      [this](const EntityDamage& damage) { announced.emplace_back(damage); });
  scene.events.subscribe<EntityDestruction>(
      [this](const EntityDestruction& destruction) {
        announced.emplace_back(destruction);
      });
  scene.events.subscribe<EnemyKilled>(
      [this](const EnemyKilled& kill) { kills.push_back(kill); });
}

engine::Entity DamageScene::missileOf(std::uint32_t networkId,
                                      MissileOwner owner) {
  const engine::Entity entity = missile(networkId);
  scene.components.add(entity, owner);
  return entity;
}

engine::Entity DamageScene::bydoWorth(std::uint32_t networkId, Health health,
                                      std::uint32_t points) {
  const engine::Entity entity = bydo(networkId, health);
  scene.components.add(entity, ScoreValue{.points = points});
  return entity;
}

engine::Entity DamageScene::missile(std::uint32_t networkId) {
  const engine::Entity entity =
      scene.spawn(EntityType::PlayerMissile, 0.0F, 0.0F);
  scene.components.add(entity, NetworkIdentity{.networkId = networkId});
  return entity;
}

engine::Entity DamageScene::bydo(std::uint32_t networkId, Health health) {
  const engine::Entity entity = scene.spawn(EntityType::Enemy, 0.0F, 0.0F);
  scene.components.add(entity, NetworkIdentity{.networkId = networkId});
  scene.components.add(entity, health);
  return entity;
}

void DamageScene::hit(engine::Entity missile, engine::Entity target) {
  scene.events.publish(MissileHit{.missile = missile, .target = target});
  scene.events.dispatch();
}

}  // namespace rtype::game::testing
