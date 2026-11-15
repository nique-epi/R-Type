#include "BossSystem.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include "ActionKey.hpp"
#include "Boss.hpp"
#include "BossHealthChanged.hpp"
#include "ComponentRegistry.hpp"
#include "CurrentAction.hpp"
#include "EnemyActionStart.hpp"
#include "EnemyActions.hpp"
#include "EnemyPhase.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EventBus.hpp"
#include "Health.hpp"
#include "NetworkIdentity.hpp"
#include "Rage.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

namespace {

/** @returns Whether the arrival of @p boss, in @p current, is over: it ended
 * an action and is not in its arrival attack any more. */
bool arrivalIsOver(const Boss& boss, const CurrentAction& current) {
  return current.ended > 0 && (!current.key.has_value() ||
                               current.key->attack != boss.arrivalAttack);
}

/** @returns The health of the boss @p body to tell the players; nothing for
 * a body without a NetworkIdentity or without Health. */
std::optional<BossHealthChanged> healthOf(engine::ComponentRegistry& components,
                                          engine::Entity body) {
  const auto* const identity = components.get<NetworkIdentity>(body);
  const auto* const health = components.get<Health>(body);
  if (identity == nullptr || health == nullptr) {
    return std::nullopt;
  }
  return BossHealthChanged{.networkId = identity->networkId,
                           .points = static_cast<std::uint16_t>(health->points),
                           .full = static_cast<std::uint16_t>(health->full),
                           .enraged = components.has<Rage>(body)};
}

}  // namespace

BossSystem::BossSystem(engine::EventBus& events)
    : events_(events),
      destructions_(events.subscribe<EntityDestruction>(
          [this](const EntityDestruction& destruction) {
            onDestruction(destruction);
          })) {}

BossSystem::~BossSystem() { events_.get().unsubscribe(destructions_); }

void BossSystem::update(engine::ComponentRegistry& components,
                        [[maybe_unused]] engine::Duration elapsed) {
  enraging_.clear();
  components.forEach<Boss, Health>([this, &components](engine::Entity body,
                                                       Boss& boss,
                                                       const Health& health) {
    const auto* const current = components.get<CurrentAction>(body);
    if (current == nullptr) {
      return;
    }
    if (!boss.fighting && arrivalIsOver(boss, *current)) {
      boss.fighting = true;
    }
    if (boss.fighting && !components.has<Rage>(body) &&
        health.points < boss.rageBelowPoints) {
      enraging_.push_back(body);
    }
  });
  for (const engine::Entity boss : enraging_) {
    enrage(components, boss);
  }
  tellChangedHealths(components);
}

void BossSystem::enrage(engine::ComponentRegistry& components,
                        engine::Entity boss) {
  const auto* const phases = components.get<Boss>(boss);
  if (phases == nullptr) {
    return;
  }
  components.add(boss, Rage{});
  startEnemyAction(
      events_.get(), components,
      EnemyActionStart{.enemy = boss,
                       .action = ActionKey{.attack = phases->rageAttack,
                                           .phase = EnemyPhase::Enraged}});
}

void BossSystem::tellChangedHealths(engine::ComponentRegistry& components) {
  healths_.clear();
  components.forEach<Boss, Health>([this, &components](engine::Entity body,
                                                       const Boss& boss,
                                                       const Health&) {
    const std::optional<BossHealthChanged> health = healthOf(components, body);
    if (boss.fighting && health.has_value()) {
      healths_.push_back(*health);
    }
  });
  for (const BossHealthChanged& health : healths_) {
    if (std::ranges::find(told_, health) == told_.end()) {
      events_.get().publish(health);
    }
  }
  told_.swap(healths_);
}

void BossSystem::onDestruction(const EntityDestruction& destruction) {
  const auto told = std::ranges::find(told_, destruction.networkId,
                                      &BossHealthChanged::networkId);
  if (told == told_.end()) {
    return;
  }
  if (destruction.cause == DestructionCause::Killed) {
    BossHealthChanged defeated = *told;
    defeated.points = 0;
    events_.get().publish(defeated);
  }
  told_.erase(told);
}

}  // namespace rtype::game
