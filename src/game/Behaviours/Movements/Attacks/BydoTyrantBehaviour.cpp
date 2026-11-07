#include "BydoTyrantBehaviour.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>
#include "ActionCycle.hpp"
#include "ActionKey.hpp"
#include "ActionTimings.hpp"
#include "BehaviourConstants.hpp"
#include "BehaviourSetting.hpp"
#include "BehaviourSteering.hpp"
#include "BehaviourView.hpp"
#include "BydoTyrantBehaviourConstants.hpp"
#include "Direction.hpp"
#include "EnemyPhase.hpp"
#include "Intent.hpp"
#include "PercentConstants.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"
#include "TyrantAttack.hpp"

namespace rtype::game {

namespace {

/** @returns The setting at @p index of @p view, in logical units. */
float unitsSettingAt(const BehaviourView& view, std::size_t index) {
  return static_cast<float>(settingAt(view, index));
}

/** @returns The attack @p attack, timed by the settings of @p view, the
 * signal of an enraged variant shortened to `rage_signal_percent`; an attack
 * of no phase, which ends at once, for a number it does not know. */
ActionTimings timingsOf(const BehaviourView& view, std::uint8_t attack) {
  const auto found =
      std::ranges::find(TYRANT_ATTACKS, attack, &TyrantAttack::attack);
  if (found == TYRANT_ATTACKS.end()) {
    return ActionTimings{.attack = attack};
  }
  ActionTimings timings = timingsAt(view, attack, found->signalSetting);
  if (found->hastened) {
    timings.signal = timings.signal *
                     settingAt(view, TYRANT_RAGE_SIGNAL_SETTING) /
                     WHOLE_PERCENT;
  }
  return timings;
}

/** @returns The column it stands at, at its age: sliding from where it
 * appeared to `rest_x` over `slide_ms`, then there. */
float columnAt(const BehaviourView& view) {
  const float restX = unitsSettingAt(view, TYRANT_REST_X_SETTING);
  const engine::Duration slide =
      millisecondsSettingAt(view, TYRANT_SLIDE_SETTING);
  if (view.age >= slide) {
    return restX;
  }
  const float share = std::chrono::duration<float>(view.age) /
                      std::chrono::duration<float>(slide);
  return view.origin.x + ((restX - view.origin.x) * share);
}

/** @returns It keeping its place: its column of the moment, on its row. */
Intent holdingAt(const BehaviourView& view) {
  return Intent{
      .direction = Direction{.x = horizontalShareTowards(columnAt(view), view),
                             .y = verticalShareTowards(view.origin.y, view)},
      .steered = true};
}

/** @returns The column its charge takes it to at this point of its strike:
 * from `rest_x` out past the left edge, then in from past the right edge back
 * to `rest_x`. */
float chargeColumnAt(const BehaviourView& view, const ActionTimings& charge) {
  const float restX = unitsSettingAt(view, TYRANT_REST_X_SETTING);
  const float progress =
      std::min(1.0F, std::chrono::duration<float>(view.actionAge) /
                         std::chrono::duration<float>(charge.strike));
  if (progress < TYRANT_CHARGE_TURN_SHARE) {
    return restX + ((TYRANT_CHARGE_LEFT_COLUMN - restX) * progress /
                    TYRANT_CHARGE_TURN_SHARE);
  }
  const float back =
      (progress - TYRANT_CHARGE_TURN_SHARE) / (1.0F - TYRANT_CHARGE_TURN_SHARE);
  return TYRANT_CHARGE_RIGHT_COLUMN +
         ((restX - TYRANT_CHARGE_RIGHT_COLUMN) * back);
}

/** @returns Its charge at this tick: towards its charge column, or a leap
 * there once that column came back on the right while it is still left of
 * the middle. */
Intent charging(const BehaviourView& view, const ActionTimings& charge) {
  const float column = chargeColumnAt(view, charge);
  if (column > TYRANT_PLAYFIELD_MIDDLE_COLUMN &&
      view.position.x < TYRANT_PLAYFIELD_MIDDLE_COLUMN) {
    return Intent{.steered = true,
                  .jumpsTo = Position{.x = column, .y = view.origin.y}};
  }
  return Intent{
      .direction = Direction{.x = horizontalShareTowards(column, view),
                             .y = verticalShareTowards(view.origin.y, view)},
      .steered = true};
}

/** @returns It holding still in its rage, which it ends after `rage_ms`. */
Intent raging(const BehaviourView& view) {
  return Intent{.steered = true,
                .endsAction = view.actionAge >=
                              millisecondsSettingAt(view, TYRANT_RAGE_SETTING)};
}

/** @returns The next step of @p action, the action it is in. */
Intent inAction(const BehaviourView& view, ActionKey action) {
  if (action.phase == EnemyPhase::Enraged) {
    return raging(view);
  }
  const ActionTimings timings = timingsOf(view, action.attack);
  if (action.attack == TYRANT_CHARGE_ATTACK &&
      action.phase == EnemyPhase::Strike) {
    return withNextPhase(charging(view, timings), view, timings);
  }
  return withNextPhase(holdingAt(view), view, timings);
}

/** @returns Whether a ship is within `melee_reach` of its melee point. */
bool isAShipInReach(const BehaviourView& view) {
  if (!view.nearestShip.has_value()) {
    return false;
  }
  const float meleeX =
      view.position.x + unitsSettingAt(view, TYRANT_MELEE_X_SETTING);
  const float meleeY =
      view.position.y + unitsSettingAt(view, TYRANT_MELEE_Y_SETTING);
  return std::hypot(view.nearestShip->x - meleeX,
                    view.nearestShip->y - meleeY) <
         unitsSettingAt(view, TYRANT_MELEE_REACH_SETTING);
}

/** @returns The attack its rotation, calm or enraged, comes to. */
std::uint8_t nextAttackOf(const BehaviourView& view) {
  const std::span<const std::uint8_t> rotation =
      view.enraged ? std::span<const std::uint8_t>{TYRANT_ENRAGED_ROTATION}
                   : std::span<const std::uint8_t>{TYRANT_CALM_ROTATION};
  return rotation[view.actionsEnded % rotation.size()];
}

/** @returns What it does while idle: arrive until it ended an action, then,
 * rested, its tentacles at a close ship or its next attack. */
Intent whenIdle(const BehaviourView& view) {
  const Intent holding = holdingAt(view);
  if (view.actionsEnded == 0) {
    return withStart(holding, timingsOf(view, TYRANT_ARRIVAL_ATTACK));
  }
  if (!hasRested(view, millisecondsSettingAt(view, TYRANT_REST_SETTING))) {
    return holding;
  }
  if (isAShipInReach(view)) {
    return withStart(
        holding, timingsOf(view, view.enraged ? TYRANT_ENRAGED_TENTACLE_ATTACK
                                              : TYRANT_TENTACLE_ATTACK));
  }
  return withStart(holding, timingsOf(view, nextAttackOf(view)));
}

}  // namespace

std::string_view BydoTyrantBehaviour::name() const {
  return BYDO_TYRANT_BEHAVIOUR_NAME;
}

std::span<const BehaviourSetting> BydoTyrantBehaviour::settings() const {
  return BYDO_TYRANT_SETTINGS;
}

Intent BydoTyrantBehaviour::steer(const BehaviourView& view) const {
  if (view.action.has_value()) {
    return inAction(view, *view.action);
  }
  return whenIdle(view);
}

}  // namespace rtype::game
