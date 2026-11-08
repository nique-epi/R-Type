#include "BossPhaseSystem.hpp"
#include <cstdint>
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
#include "TimeConstants.hpp"

namespace rtype::game {

namespace {

/** @returns Whether the arrival of @p boss, in @p current, is over: it ended
 * an action and is not in its arrival attack any more. */
bool arrivalIsOver(const Boss& boss, const CurrentAction& current) {
  return current.ended > 0 && (!current.key.has_value() ||
                               current.key->attack != boss.arrivalAttack);
}

BossHealthChanged healthOf(const NetworkIdentity& identity, const Boss& boss,
                           const Health& health) {
  return BossHealthChanged{.networkId = identity.networkId,
                           .points = static_cast<std::uint16_t>(health.points),
                           .full = static_cast<std::uint16_t>(health.full),
                           .enraged = boss.enraged};
}

}  // namespace

BossPhaseSystem::BossPhaseSystem(engine::EventBus& events) : events_(events) {}

void BossPhaseSystem::update(engine::ComponentRegistry& components,
                             [[maybe_unused]] engine::Duration elapsed) {
  components.forEach<Boss, Health>([this, &components](engine::Entity body,
                                                       Boss& boss,
                                                       const Health& health) {
    const auto* const current = components.get<CurrentAction>(body);
    const auto* const identity = components.get<NetworkIdentity>(body);
    if (current == nullptr || identity == nullptr) {
      return;
    }
    if (!boss.fighting && arrivalIsOver(boss, *current)) {
      boss.fighting = true;
      events_.get().publish(healthOf(*identity, boss, health));
    }
    if (!boss.fighting || boss.enraged ||
        health.points >= boss.rageBelowPoints) {
      return;
    }
    boss.enraged = true;
    startEnemyAction(
        events_.get(), components,
        EnemyActionStart{.enemy = body,
                         .action = ActionKey{.attack = boss.rageAttack,
                                             .phase = EnemyPhase::Enraged}});
    events_.get().publish(healthOf(*identity, boss, health));
  });
}

}  // namespace rtype::game
