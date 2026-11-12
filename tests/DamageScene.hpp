#pragma once

#include <cstdint>
#include <variant>
#include <vector>
#include "Boss.hpp"
#include "BossHealthChanged.hpp"
#include "CollisionScene.hpp"
#include "CurrentAction.hpp"
#include "DamageRules.hpp"
#include "EnemyKilled.hpp"
#include "Entity.hpp"
#include "EntityDamage.hpp"
#include "EntityDestruction.hpp"
#include "Health.hpp"
#include "IHitHandler.hpp"
#include "MissileOwner.hpp"

namespace rtype::game::testing {

/** @brief What DamageRules announces about one entity: a damage or a
 * destruction. */
using Announcement = std::variant<EntityDamage, EntityDestruction>;

/**
 * @brief A CollisionScene ruled by DamageRules, recording what it announces,
 * the kills it credits and the health of the bosses it announces, in the
 * order they are published.
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
  /** @returns A missile of no player that takes @p points from what it hits,
   * as a sheet naming `[gameplay.damage]` makes it. */
  engine::Entity missileDealing(std::uint32_t networkId, int points);
  /** @returns A Bydo with @p health, worth nothing. */
  engine::Entity bydo(std::uint32_t networkId, Health health);
  /** @returns A boss with @p health, its Boss @p boss, in the action
   * @p current, worth nothing; its network id is BOSS_ID. */
  engine::Entity boss(Health health, Boss boss, CurrentAction current);
  /** @returns A Shootable missile of an enemy. */
  engine::Entity shootableMissile(std::uint32_t networkId);
  /** @returns An obstacle without Health, as a wall of steel is. */
  engine::Entity steelWall(std::uint32_t networkId);
  /** @brief Makes @p handler told of the hits applied to @p body, as the
   * sheet of a body that names an on_hit makes it. */
  void reactingTo(engine::Entity body, IHitHandler& handler);
  /** @brief Publishes that @p missile hit @p target, and delivers it. */
  void hit(engine::Entity missile, engine::Entity target);

  CollisionScene scene;
  DamageRules rules{scene.events, scene.components};
  std::vector<Announcement> announced;
  std::vector<EnemyKilled> kills;
  std::vector<BossHealthChanged> bossHealths;
};

}  // namespace rtype::game::testing
