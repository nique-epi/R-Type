#pragma once

#include <cstdint>
#include <optional>
#include "Entity.hpp"
#include "SubscriptionHandle.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EventBus;

}  // namespace rtype::engine

namespace rtype::game {

struct AppliedHit;
struct Health;
struct MissileHit;

/**
 * @brief Decides what a hit costs: the missile disappears and the target loses
 * health, or is destroyed when none is left.
 *
 * Subscribes to MissileHit on construction and unsubscribes on destruction.
 * The missile is always destroyed and announced with an EntityDestruction
 * Removed. A target with Health loses the points of the MissileDamage of the
 * missile, MISSILE_DAMAGE for a missile without one, or all of them while it
 * is in the action its Fragile names: it is announced
 * with an EntityDamage while it has points left, and destroyed with an
 * EntityDestruction Killed when it has none. A target without Health is not
 * hurt, unless it is a Shootable missile of an enemy, which is destroyed
 * with an EntityDestruction Killed. A Boss in its arrival loses nothing: the
 * missile is removed, nothing else is announced. A Boss that loses points is
 * announced with a BossHealthChanged. When the missile has a MissileOwner and
 * the target that dies has a ScoreValue above 0, an EnemyKilled follows the
 * destruction, naming the owner and the points. A hit naming an entity that is
 * gone or has no NetworkIdentity is ignored, so two missiles reaching the same
 * target in one dispatch never destroy it twice. Once a hit is applied and its
 * announcements published, the HitReaction of the target, when its sheet
 * names an on_hit, is told of it as an AppliedHit, a hit on a target without
 * Health included: what the handler publishes is delivered after the
 * announcements of the hit, since the bus delivers in the order of
 * publication. The bus and the registry must outlive this object.
 */
class DamageRules {
 public:
  DamageRules(engine::EventBus& events, engine::ComponentRegistry& components);
  ~DamageRules();

  DamageRules(const DamageRules&) = delete;
  DamageRules& operator=(const DamageRules&) = delete;
  DamageRules(DamageRules&&) = delete;
  DamageRules& operator=(DamageRules&&) = delete;

 private:
  void onMissileHit(const MissileHit& hit);
  /** @brief Takes from @p target what the hit @p applied names, if it has
   * Health, and announces it as the rules of the class say; @p applied then
   * holds what the hit took, what @p target has left and whether it died. */
  void hurtTarget(engine::Entity target, std::optional<std::uint8_t> shooter,
                  AppliedHit& applied);
  /** @brief Destroys @p missile and announces it Removed. */
  void removeMissile(engine::Entity missile, std::uint32_t networkId);
  /** @brief Destroys @p target, announces it Killed, and credits its points
   * to @p shooter, when a player shot it and it is worth any. */
  void killTarget(engine::Entity target, std::uint32_t networkId,
                  std::optional<std::uint8_t> shooter);

  /** @brief Publishes a BossHealthChanged for @p target, which has
   * @p health, when it is a boss. */
  void announceBossHealth(engine::Entity target, std::uint32_t networkId,
                          const Health& health);

  engine::EventBus& events_;
  engine::ComponentRegistry& components_;
  engine::SubscriptionHandle subscription_;
};

}  // namespace rtype::game
