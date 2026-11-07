#pragma once

#include "Collision.hpp"
#include "MissileHit.hpp"
#include "SubscriptionHandle.hpp"
#include "TypedEntity.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EventBus;

}  // namespace rtype::engine

namespace rtype::game {

/**
 * @brief Decides which collisions matter and announces them as game events.
 *
 * Subscribes to engine::Collision on construction and unsubscribes on
 * destruction. A player missile and an enemy, or an enemy missile and a
 * player, become a MissileHit, except a player missile that carries a
 * ViewDelay: LagCompensatedHitSystem tests that one where its player saw
 * the enemies. A player and an enemy become a ShipContact. With friendly
 * fire on, a player missile and the ship of another player become a
 * MissileHit too: the MissileOwner of the missile and the ShipPilot of the
 * ship name two players. A player and a look item become a LookItemTouched.
 * Any other pair, a missile of no player, a ship of
 * no pilot, an entity without an EntityType and an entity that is no longer
 * alive are ignored. The two entities of a Collision may come in either order.
 * The bus and the registry must outlive this object.
 */
class CollisionRules {
 public:
  CollisionRules(engine::EventBus& events,
                 engine::ComponentRegistry& components,
                 bool friendlyFire = false);
  ~CollisionRules();

  CollisionRules(const CollisionRules&) = delete;
  CollisionRules& operator=(const CollisionRules&) = delete;
  CollisionRules(CollisionRules&&) = delete;
  CollisionRules& operator=(CollisionRules&&) = delete;

 private:
  void onCollision(const engine::Collision& collision);
  /** @brief Publishes what @p striker touching @p struck means, if anything:
   * a MissileHit or a ShipContact. */
  void publishIfRuleMatches(TypedEntity striker, TypedEntity struck);
  /** @returns Whether friendly fire is on and the missile of @p hit, a player
   * missile, belongs to another player than the pilot of its target, a player
   * ship. */
  [[nodiscard]] bool hitsAnotherPilot(const MissileHit& hit) const;

  engine::EventBus& events_;
  engine::ComponentRegistry& components_;
  bool friendlyFire_{false};
  engine::SubscriptionHandle subscription_;
};

}  // namespace rtype::game
