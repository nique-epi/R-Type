#pragma once

#include "Collision.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "SubscriptionHandle.hpp"

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
 * player, become a MissileHit; a player and an enemy become a ShipContact.
 * With friendly fire on, a player missile and the ship of another player
 * become a MissileHit too: the MissileOwner of the missile and the ShipPilot
 * of the ship name two players. Any other pair, a missile of no player, a ship
 * of no pilot, an entity without an EntityType and an entity that is no longer
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
  void publishIfRuleMatches(engine::Entity first, EntityType firstType,
                            engine::Entity second, EntityType secondType);
  /** @returns Whether friendly fire is on and @p missile, a player missile,
   * belongs to another player than the pilot of @p ship, a player ship. */
  [[nodiscard]] bool hitsAnotherPilot(engine::Entity missile,
                                      engine::Entity ship) const;

  engine::EventBus& events_;
  engine::ComponentRegistry& components_;
  bool friendlyFire_{false};
  engine::SubscriptionHandle subscription_;
};

}  // namespace rtype::game
