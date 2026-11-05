#include "ActionCycle.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include "ActionKey.hpp"
#include "ActionTimings.hpp"
#include "BehaviourSteering.hpp"
#include "BehaviourView.hpp"
#include "EnemyPhase.hpp"
#include "Intent.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

namespace {

/** @returns How long @p phase of @p timings lasts; nothing for a phase the
 * cycle does not time (Enraged). */
std::optional<engine::Duration> lengthOf(const ActionTimings& timings,
                                         EnemyPhase phase) {
  switch (phase) {
    case EnemyPhase::Signal:
      return timings.signal;
    case EnemyPhase::Strike:
      return timings.strike;
    case EnemyPhase::Recover:
      return timings.recover;
    case EnemyPhase::Enraged:
      return std::nullopt;
  }
  return std::nullopt;
}

/** @returns The first phase of @p timings after @p phase that lasts; nothing
 * when none does, the action then ends. */
std::optional<EnemyPhase> phaseAfter(const ActionTimings& timings,
                                     EnemyPhase phase) {
  if (phase == EnemyPhase::Signal &&
      timings.strike > engine::Duration::zero()) {
    return EnemyPhase::Strike;
  }
  const bool beforeRecover =
      phase == EnemyPhase::Signal || phase == EnemyPhase::Strike;
  if (beforeRecover && timings.recover > engine::Duration::zero()) {
    return EnemyPhase::Recover;
  }
  return std::nullopt;
}

}  // namespace

ActionTimings timingsAt(const BehaviourView& view, std::uint8_t attack,
                        std::size_t signalSetting) {
  return ActionTimings{
      .attack = attack,
      .signal = millisecondsSettingAt(view, signalSetting),
      .strike = millisecondsSettingAt(view, signalSetting + 1),
      .recover = millisecondsSettingAt(view, signalSetting + 2)};
}

Intent withStart(Intent intent, const ActionTimings& timings) {
  EnemyPhase first = EnemyPhase::Recover;
  if (timings.signal > engine::Duration::zero()) {
    first = EnemyPhase::Signal;
  } else if (timings.strike > engine::Duration::zero()) {
    first = EnemyPhase::Strike;
  }
  intent.startsAction = ActionKey{.attack = timings.attack, .phase = first};
  return intent;
}

Intent withNextPhase(Intent intent, const BehaviourView& view,
                     const ActionTimings& timings) {
  if (!view.action.has_value() || view.action->attack != timings.attack) {
    return intent;
  }
  const std::optional<engine::Duration> length =
      lengthOf(timings, view.action->phase);
  if (!length.has_value() || view.actionAge < *length) {
    return intent;
  }
  const std::optional<EnemyPhase> next =
      phaseAfter(timings, view.action->phase);
  if (next.has_value()) {
    intent.startsAction = ActionKey{.attack = timings.attack, .phase = *next};
  } else {
    intent.endsAction = true;
  }
  return intent;
}

bool hasRested(const BehaviourView& view, engine::Duration rest) {
  return !view.action.has_value() && view.actionAge >= rest;
}

}  // namespace rtype::game
