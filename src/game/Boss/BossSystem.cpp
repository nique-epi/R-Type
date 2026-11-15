#include "BossSystem.hpp"
#include <algorithm>
#include <cstdint>
#include <optional>
#include <utility>
#include "ActionKey.hpp"
#include "Boss.hpp"
#include "BossArrival.hpp"
#include "BossConstants.hpp"
#include "BossHealth.hpp"
#include "BossHealthChanged.hpp"
#include "BossHealthScaling.hpp"
#include "BossProgress.hpp"
#include "BossSystemParts.hpp"
#include "ComponentRegistry.hpp"
#include "CurrentAction.hpp"
#include "EnemyActionStart.hpp"
#include "EnemyActions.hpp"
#include "EnemyPhase.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntityTypeNumber.hpp"
#include "EventBus.hpp"
#include "Health.hpp"
#include "NetworkIdentity.hpp"
#include "PercentConstants.hpp"
#include "Rage.hpp"
#include "Summoning.hpp"
#include "SummoningStart.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

namespace {

/** @brief What a boss that moved on asks of the system, once the visit of
 * the components is over. */
enum class BossChange : std::uint8_t {
  None,
  Warned,
  Woken,
};

/** @returns Whether the arrival of @p boss, the body @p body, is over: it
 * ended an action and is not in its arrival attack any more. */
bool arrivalIsOver(engine::ComponentRegistry& components, engine::Entity body,
                   const Boss& boss) {
  const auto* const current = components.get<CurrentAction>(body);
  return current != nullptr && current->ended > 0 &&
         (!current->key.has_value() ||
          current->key->attack != boss.arrivalAttack);
}

/** @returns Whether @p health is below the share of its full points @p boss
 * enrages below, the `rage_below_percent` of its sheet, rounded down. */
bool isBelowItsRage(const Boss& boss, const Health& health) {
  return health.points < health.full * boss.rageBelowPercent / WHOLE_PERCENT;
}

/** @brief Moves @p boss, the body @p body, on to the progress it has
 * reached, and @returns what that asks of the system. */
BossChange advance(engine::ComponentRegistry& components, engine::Entity body,
                   Boss& boss) {
  if (boss.progress == BossProgress::Created) {
    boss.progress = BossProgress::Warned;
    return BossChange::Warned;
  }
  if (boss.progress == BossProgress::Warned &&
      !components.has<Summoning>(body)) {
    boss.progress = BossProgress::Arriving;
    return BossChange::Woken;
  }
  if (boss.progress == BossProgress::Arriving &&
      arrivalIsOver(components, body, boss)) {
    boss.progress = BossProgress::Fighting;
  }
  return BossChange::None;
}

/** @returns What to tell the players of the boss @p body, which has
 * @p health; nothing for a body without a NetworkIdentity. */
std::optional<BossHealthChanged> toldHealthOf(
    engine::ComponentRegistry& components, engine::Entity body,
    const Health& health) {
  const auto* const identity = components.get<NetworkIdentity>(body);
  if (identity == nullptr) {
    return std::nullopt;
  }
  return BossHealthChanged{.networkId = identity->networkId,
                           .points = static_cast<std::uint16_t>(health.points),
                           .full = static_cast<std::uint16_t>(health.full),
                           .enraged = components.has<Rage>(body)};
}

}  // namespace

BossSystem::BossSystem(BossSystemParts parts)
    : parts_(std::move(parts)),
      destructions_(parts_.events.get().subscribe<EntityDestruction>(
          [this](const EntityDestruction& destruction) {
            onDestruction(destruction);
          })) {}

BossSystem::~BossSystem() { parts_.events.get().unsubscribe(destructions_); }

void BossSystem::update(engine::ComponentRegistry& components,
                        [[maybe_unused]] engine::Duration elapsed) {
  warned_.clear();
  enraging_.clear();
  components.forEach<Boss, Health>(
      [this, &components](engine::Entity body, Boss& boss, Health& health) {
        const BossChange change = advance(components, body, boss);
        if (change == BossChange::Warned) {
          warned_.push_back(body);
        } else if (change == BossChange::Woken) {
          strengthen(health);
        } else if (boss.progress == BossProgress::Fighting &&
                   isBelowItsRage(boss, health) &&
                   !components.has<Rage>(body)) {
          enraging_.push_back(body);
        }
      });
  for (const engine::Entity boss : warned_) {
    warn(components, boss);
  }
  for (const engine::Entity boss : enraging_) {
    enrage(components, boss);
  }
  tellChangedHealths(components);
}

void BossSystem::warn(engine::ComponentRegistry& components,
                      engine::Entity boss) const {
  startSummoning(components, boss, BOSS_ARRIVAL_WARNING_TICKS);
  const auto* const type = components.get<EntityTypeNumber>(boss);
  parts_.events.get().publish(BossArrival{
      .typeNumber = type == nullptr ? std::uint8_t{0} : type->number,
      .warningMilliseconds = BOSS_ARRIVAL_WARNING_MILLISECONDS});
}

void BossSystem::strengthen(Health& health) const {
  health.full = scaledBossHealth(
      BossHealthScaling{.points = health.full,
                        .players = parts_.players(),
                        .healthPercent = parts_.healthPercent});
  health.points = health.full;
}

void BossSystem::enrage(engine::ComponentRegistry& components,
                        engine::Entity boss) const {
  const auto* const phases = components.get<Boss>(boss);
  if (phases == nullptr) {
    return;
  }
  components.add(boss, Rage{});
  startEnemyAction(
      parts_.events.get(), components,
      EnemyActionStart{.enemy = boss,
                       .action = ActionKey{.attack = phases->rageAttack,
                                           .phase = EnemyPhase::Enraged}});
}

void BossSystem::tellChangedHealths(engine::ComponentRegistry& components) {
  healths_.clear();
  components.forEach<Boss, Health>([this, &components](engine::Entity body,
                                                       const Boss& boss,
                                                       const Health& health) {
    if (boss.progress != BossProgress::Fighting) {
      return;
    }
    const std::optional<BossHealthChanged> told =
        toldHealthOf(components, body, health);
    if (told.has_value()) {
      healths_.push_back(*told);
    }
  });
  for (const BossHealthChanged& health : healths_) {
    if (std::ranges::find(told_, health) == told_.end()) {
      parts_.events.get().publish(health);
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
    parts_.events.get().publish(defeated);
  }
  told_.erase(told);
}

}  // namespace rtype::game
