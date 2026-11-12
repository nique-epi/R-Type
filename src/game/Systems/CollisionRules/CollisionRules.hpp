#pragma once

#include <optional>
#include "CollidingEntity.hpp"
#include "Collision.hpp"
#include "Entity.hpp"
#include "MissileHit.hpp"
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
 * destruction. Which contacts count comes from the sheets: a pair is kept
 * when the engine::CollisionFilter of one hits a layer the other lies in
 * (engine::mayCollide()), whichever of the two says so. What a kept contact
 * means comes from the EntityType of the two:
 * - a missile and anything but a missile become a MissileHit, the other being
 *   the target;
 * - two missiles become a MissileHit for each one that hits the layer of the
 *   other, so a missile may be stopped by one it does not stop;
 * - a player and an enemy become a ShipContact;
 * - a player and a look item become a LookItemTouched.
 * A missile of a player and the ship of a player become a MissileHit only
 * with friendly fire on, when the MissileOwner of the missile and the
 * ShipPilot of the ship name two players: a missile of no player and a ship
 * of no pilot are left alone. A missile of a player that carries a ViewDelay
 * is not judged here against an enemy, nor a ship that carries one against an
 * enemy or the missile of an enemy: LagCompensatedHitSystem tests those where
 * their player saw the enemies. A MissileHit is published only when its
 * target lets it (targetLetsHit()): the behaviour of a piloted body may
 * cancel a hit before it is announced, and is asked again at each Collision
 * of the same pair. Any other pair, an entity without an
 * EntityType or without a filter and an entity that is no longer alive are
 * ignored. The two entities of a Collision may come in either order.
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
  /** @returns @p entity with its side and its filter; nothing when it has
   * lost one of them, or never had it. */
  [[nodiscard]] std::optional<CollidingEntity> collidingEntityOf(
      engine::Entity entity) const;
  /** @brief Publishes what the kept contact of @p one with @p other means
   * for @p one: the MissileHit of a missile, or the ShipContact or the
   * LookItemTouched of a ship; nothing for any other body. */
  void publishContactOf(CollidingEntity one, CollidingEntity other);
  /** @returns Whether @p missile touching @p target is a hit to announce
   * here: see the rules of the class for another missile, the ship of a
   * player and a contact LagCompensatedHitSystem tests. */
  [[nodiscard]] bool isAHitOf(CollidingEntity missile,
                              CollidingEntity target) const;
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
