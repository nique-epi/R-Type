#include "BossPhaseSystem.hpp"
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

/** @returns The health of the boss @p body to announce; nothing for a body
 * without the parts of a boss. */
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

/** @brief Publishes the health of the boss @p body on @p events. */
void announceHealthOf(engine::EventBus& events,
                      engine::ComponentRegistry& components,
                      engine::Entity body) {
  const std::optional<BossHealthChanged> health = healthOf(components, body);
  if (health.has_value()) {
    events.publish(*health);
  }
}

}  // namespace

BossPhaseSystem::BossPhaseSystem(engine::EventBus& events) : events_(events) {}

void BossPhaseSystem::update(engine::ComponentRegistry& components,
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
      announceHealthOf(events_.get(), components, body);
    }
    if (boss.fighting && !components.has<Rage>(body) &&
        health.points < boss.rageBelowPoints) {
      enraging_.push_back(body);
    }
  });
  for (const engine::Entity boss : enraging_) {
    enrage(components, boss);
  }
}

void BossPhaseSystem::enrage(engine::ComponentRegistry& components,
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
  announceHealthOf(events_.get(), components, boss);
}

}  // namespace rtype::game
