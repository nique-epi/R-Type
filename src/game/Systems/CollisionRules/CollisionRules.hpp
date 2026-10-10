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
 * player, become a MissileHit; a player and an enemy become a ShipContact. Any
 * other pair, an entity without an EntityType and an entity that is no longer
 * alive are ignored. The two entities of a Collision may come in either order.
 * The bus and the registry must outlive this object.
 */
class CollisionRules {
 public:
  CollisionRules(engine::EventBus& events,
                 engine::ComponentRegistry& components);
  ~CollisionRules();

  CollisionRules(const CollisionRules&) = delete;
  CollisionRules& operator=(const CollisionRules&) = delete;
  CollisionRules(CollisionRules&&) = delete;
  CollisionRules& operator=(CollisionRules&&) = delete;

 private:
  void onCollision(const engine::Collision& collision);
  void publishIfRuleMatches(engine::Entity first, EntityType firstType,
                            engine::Entity second, EntityType secondType);

  engine::EventBus& events_;
  engine::ComponentRegistry& components_;
  engine::SubscriptionHandle subscription_;
};

}  // namespace rtype::game
