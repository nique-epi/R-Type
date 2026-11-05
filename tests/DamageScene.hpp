#pragma once

#include <cstdint>
#include <variant>
#include <vector>
#include "CollisionScene.hpp"
#include "DamageRules.hpp"
#include "EnemyKilled.hpp"
#include "Entity.hpp"
#include "EntityDamage.hpp"
#include "EntityDestruction.hpp"
#include "Health.hpp"
#include "MissileOwner.hpp"

namespace rtype::game::testing {

/** @brief What DamageRules announces about one entity: a damage or a
 * destruction. */
using Announcement = std::variant<EntityDamage, EntityDestruction>;

/**
 * @brief A CollisionScene ruled by DamageRules, recording what it announces
 * and the kills it credits, in the order they are published.
 */
struct DamageScene {
  DamageScene();

  /** @returns A missile of @p owner, with the network id @p networkId. */
  engine::Entity missileOf(std::uint32_t networkId, MissileOwner owner);
  /** @returns A Bydo with @p health, worth @p points. */
  engine::Entity bydoWorth(std::uint32_t networkId, Health health,
                           std::uint32_t points);
  /** @returns A missile of no player. */
  engine::Entity missile(std::uint32_t networkId);
  /** @returns A Bydo with @p health, worth nothing. */
  engine::Entity bydo(std::uint32_t networkId, Health health);
  /** @brief Publishes that @p missile hit @p target, and delivers it. */
  void hit(engine::Entity missile, engine::Entity target);

  CollisionScene scene;
  DamageRules rules{scene.events, scene.components};
  std::vector<Announcement> announced;
  std::vector<EnemyKilled> kills;
};

}  // namespace rtype::game::testing
