#pragma once

#include "SubscriptionHandle.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EventBus;

}  // namespace rtype::engine

namespace rtype::game {

struct MissileHit;

/**
 * @brief Decides what a hit costs: the missile disappears and the target loses
 * health, or is destroyed when none is left.
 *
 * Subscribes to MissileHit on construction and unsubscribes on destruction.
 * The missile is always destroyed and announced with an EntityDestruction
 * Removed. A target with Health loses MISSILE_DAMAGE points: it is announced
 * with an EntityDamage while it has points left, and destroyed with an
 * EntityDestruction Killed when it has none. A target without Health is not
 * hurt. A hit naming an entity that is gone or has no NetworkIdentity is
 * ignored, so two missiles reaching the same target in one dispatch never
 * destroy it twice. The bus and the registry must outlive this object.
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

  engine::EventBus& events_;
  engine::ComponentRegistry& components_;
  engine::SubscriptionHandle subscription_;
};

}  // namespace rtype::game
