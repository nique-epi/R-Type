#include "DamageRules.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>
#include "AppliedHit.hpp"
#include "Armored.hpp"
#include "ComponentRegistry.hpp"
#include "CurrentAction.hpp"
#include "DamageConstants.hpp"
#include "DamageTally.hpp"
#include "EnemyKilled.hpp"
#include "Entity.hpp"
#include "EntityDamage.hpp"
#include "EntityDestruction.hpp"
#include "EventBus.hpp"
#include "Fragile.hpp"
#include "GameRuleConstants.hpp"
#include "Health.hpp"
#include "HitReaction.hpp"
#include "IHitHandler.hpp"
#include "MissileDamage.hpp"
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

/** @brief Adds @p taken to @p tally for @p shooter: nothing for a body that
 * keeps no tally, nor for a missile of no pilot. */
void tallyDamage(DamageTally* tally, std::optional<std::uint8_t> shooter,
                 int taken) {
  if (tally == nullptr || !shooter.has_value() ||
      *shooter < identifiers::FIRST_PLAYER_NUMBER ||
      *shooter >=
          identifiers::FIRST_PLAYER_NUMBER + identifiers::MAXIMUM_PLAYERS ||
      taken <= 0) {
    return;
  }
  tally->byPilot.at(*shooter - identifiers::FIRST_PLAYER_NUMBER) +=
      static_cast<std::uint32_t>(taken);
}

/** @returns What @p score is worth; nothing for a body worth nothing. */
ScoreValue valueOf(const ScoreValue* score) {
  return score == nullptr ? ScoreValue{} : *score;
}

/** @returns Whether @p target is Armored and in any phase of the attack it
 * is armored in. */
bool isArmored(engine::ComponentRegistry& components, engine::Entity target) {
  const auto* const armored = components.get<Armored>(target);
  const auto* const current = components.get<CurrentAction>(target);
  return armored != nullptr && current != nullptr && current->key.has_value() &&
         current->key->attack == armored->attack;
}

/** @returns The points @p missile takes from what it hits: those of its
 * MissileDamage, MISSILE_DAMAGE without one. */
int missileDamageOf(engine::ComponentRegistry& components,
                    engine::Entity missile) {
  const auto* const damage = components.get<MissileDamage>(missile);
  return damage == nullptr ? MISSILE_DAMAGE : damage->points;
}

/** @returns What a hit dealing @p dealt takes from @p target, which has
 * Health: nothing while it is armored, every point left while it is Fragile
 * and in the action it is fragile in, @p dealt otherwise. */
int damageOf(engine::ComponentRegistry& components, engine::Entity target,
             int dealt) {
  if (isArmored(components, target)) {
    return 0;
  }
  const auto* const fragile = components.get<Fragile>(target);
  const auto* const current = components.get<CurrentAction>(target);
  if (fragile == nullptr || current == nullptr || current->key != fragile->in) {
    return dealt;
  }
  return components.get<Health>(target)->points;
}

/** @returns The handler the sheet of @p target names in `on_hit`; nullptr for
 * a target without one. */
IHitHandler* reactionOf(engine::ComponentRegistry& components,
                        engine::Entity target) {
  const auto* const reaction = components.get<HitReaction>(target);
  return reaction == nullptr ? nullptr : reaction->handler;
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
  IHitHandler* const reaction = reactionOf(components_, hit.target);
  AppliedHit applied{.targetNetworkId = ids->target,
                     .missileNetworkId = ids->missile,
                     .damage = missileDamageOf(components_, hit.missile)};
  removeMissile(hit.missile, ids->missile);
  hurtTarget(hit.target, shooter, applied);
  if (reaction != nullptr) {
    reaction->onHit(applied);
  }
}

void DamageRules::hurtTarget(engine::Entity target,
                             std::optional<std::uint8_t> shooter,
                             AppliedHit& applied) {
  if (components_.has<Shootable>(target)) {
    applied.damage = 0;
    applied.destroyed = true;
    killTarget(target, applied.targetNetworkId, shooter);
    return;
  }
  auto* const health = components_.get<Health>(target);
  if (health == nullptr) {
    applied.damage = 0;
    return;
  }
  const int dealt = applied.damage;
  applied.damage = damageOf(components_, target, dealt);
  health->points -= applied.damage;
  applied.healthLeft = std::max(health->points, 0);
  if (applied.damage == 0) {
    return;
  }
  tallyDamage(components_.get<DamageTally>(target), shooter,
              applied.damage + std::min(health->points, 0));
  if (health->points > 0) {
    events_.publish(EntityDamage{.networkId = applied.targetNetworkId});
    return;
  }
  applied.destroyed = true;
  killTarget(target, applied.targetNetworkId, shooter);
}

void DamageRules::removeMissile(engine::Entity missile,
                                std::uint32_t networkId) {
  components_.destroy(missile);
  events_.publish(EntityDestruction{.networkId = networkId,
                                    .cause = DestructionCause::Removed});
}

void DamageRules::killTarget(engine::Entity target, std::uint32_t networkId,
                             std::optional<std::uint8_t> shooter) {
  const ScoreValue worth = valueOf(components_.get<ScoreValue>(target));
  const DamageTally* const tally = components_.get<DamageTally>(target);
  const DamageTally damage = tally == nullptr ? DamageTally{} : *tally;
  components_.destroy(target);
  events_.publish(EntityDestruction{.networkId = networkId,
                                    .cause = DestructionCause::Killed});
  if (shooter.has_value() && (worth.points > 0 || worth.bounty > 0)) {
    events_.publish(EnemyKilled{.playerNumber = *shooter,
                                .points = worth.points,
                                .damageByPilot = damage.byPilot,
                                .bounty = worth.bounty});
  }
}

}  // namespace rtype::game
