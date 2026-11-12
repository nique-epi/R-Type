#include "DamageRules.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include "Boss.hpp"
#include "BossHealthChanged.hpp"
#include "ComponentRegistry.hpp"
#include "CurrentAction.hpp"
#include "DamageConstants.hpp"
#include "EnemyKilled.hpp"
#include "Entity.hpp"
#include "EntityDamage.hpp"
#include "EntityDestruction.hpp"
#include "EventBus.hpp"
#include "Fragile.hpp"
#include "Health.hpp"
#include "MissileHit.hpp"
#include "MissileOwner.hpp"
#include "NetworkIdentity.hpp"
#include "ScoreValue.hpp"
#include "Shootable.hpp"

namespace rtype::game {

namespace {

/** @brief The network ids of the missile and the target of one hit. */
struct HitIds {
  std::uint32_t missile{0};
  std::uint32_t target{0};
};

/** @returns The network ids of the missile and the target of @p hit; nothing
 * when either is gone or has no NetworkIdentity. */
std::optional<HitIds> idsOf(engine::ComponentRegistry& components,
                            const MissileHit& hit) {
  const auto* const missile = components.get<NetworkIdentity>(hit.missile);
  const auto* const target = components.get<NetworkIdentity>(hit.target);
  if (missile == nullptr || target == nullptr) {
    return std::nullopt;
  }
  return HitIds{.missile = missile->networkId, .target = target->networkId};
}

/** @returns The player who fired @p missile; nothing for a missile of no
 * player. */
std::optional<std::uint8_t> shooterOf(engine::ComponentRegistry& components,
                                      engine::Entity missile) {
  const auto* const owner = components.get<MissileOwner>(missile);
  if (owner == nullptr) {
    return std::nullopt;
  }
  return owner->playerNumber;
}

/** @returns Whether @p target is a boss in any phase of its arrival. */
bool isArmored(engine::ComponentRegistry& components, engine::Entity target) {
  const auto* const boss = components.get<Boss>(target);
  const auto* const current = components.get<CurrentAction>(target);
  return boss != nullptr && current != nullptr && current->key.has_value() &&
         current->key->attack == boss->arrivalAttack;
}

/** @returns What a hit takes from @p target, which has @p health: nothing
 * while it is armored, MISSILE_DAMAGE, or every point left while @p target is
 * Fragile and in the action it is fragile in. */
int damageOf(engine::ComponentRegistry& components, engine::Entity target,
             const Health& health) {
  if (isArmored(components, target)) {
    return 0;
  }
  const auto* const fragile = components.get<Fragile>(target);
  const auto* const current = components.get<CurrentAction>(target);
  if (fragile == nullptr || current == nullptr || current->key != fragile->in) {
    return MISSILE_DAMAGE;
  }
  return health.points;
}

}  // namespace

DamageRules::DamageRules(engine::EventBus& events,
                         engine::ComponentRegistry& components)
    : events_(events),
      components_(components),
      subscription_(events.subscribe<MissileHit>(
          [this](const MissileHit& hit) { onMissileHit(hit); })) {}

DamageRules::~DamageRules() { events_.unsubscribe(subscription_); }

void DamageRules::onMissileHit(const MissileHit& hit) {
  const std::optional<HitIds> ids = idsOf(components_, hit);
  if (!ids.has_value()) {
    return;
  }
  const std::optional<std::uint8_t> shooter =
      shooterOf(components_, hit.missile);
  removeMissile(hit.missile, ids->missile);
  if (components_.has<Shootable>(hit.target)) {
    killTarget(hit.target, ids->target, shooter);
    return;
  }
  auto* const health = components_.get<Health>(hit.target);
  if (health == nullptr) {
    return;
  }
  const int damage = damageOf(components_, hit.target, *health);
  if (damage == 0) {
    return;
  }
  health->points -= damage;
  announceBossHealth(hit.target, ids->target, *health);
  if (health->points > 0) {
    events_.publish(EntityDamage{.networkId = ids->target});
    return;
  }
  killTarget(hit.target, ids->target, shooter);
}

void DamageRules::removeMissile(engine::Entity missile,
                                std::uint32_t networkId) {
  components_.destroy(missile);
  events_.publish(EntityDestruction{.networkId = networkId,
                                    .cause = DestructionCause::Removed});
}

void DamageRules::killTarget(engine::Entity target, std::uint32_t networkId,
                             std::optional<std::uint8_t> shooter) {
  const ScoreValue* const score = components_.get<ScoreValue>(target);
  const std::uint32_t points = score == nullptr ? 0 : score->points;
  components_.destroy(target);
  events_.publish(EntityDestruction{.networkId = networkId,
                                    .cause = DestructionCause::Killed});
  if (shooter.has_value() && points > 0) {
    events_.publish(EnemyKilled{.playerNumber = *shooter, .points = points});
  }
}

void DamageRules::announceBossHealth(engine::Entity target,
                                     std::uint32_t networkId,
                                     const Health& health) {
  const auto* const boss = components_.get<Boss>(target);
  if (boss == nullptr) {
    return;
  }
  events_.publish(BossHealthChanged{
      .networkId = networkId,
      .points = static_cast<std::uint16_t>(std::max(health.points, 0)),
      .full = static_cast<std::uint16_t>(health.full),
      .enraged = boss->enraged});
}

}  // namespace rtype::game
