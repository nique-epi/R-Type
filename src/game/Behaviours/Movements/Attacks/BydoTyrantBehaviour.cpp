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

/** @returns The attack of number @p attack among TYRANT_ATTACKS, nullptr for
 * a number it does not know. */
const TyrantAttack* tyrantAttackOf(std::uint8_t attack) {
  for (const TyrantAttack& candidate : TYRANT_ATTACKS) {
    if (candidate.attack == attack) {
      return &candidate;
    }
  }
  return nullptr;
}

/** @returns The attack @p attack, timed by the settings of @p view, the
 * signal of an enraged variant shortened to `rage_signal_percent`; an attack
 * of no phase, which ends at once, for a number it does not know. */
ActionTimings timingsOf(const BehaviourView& view, std::uint8_t attack) {
  const TyrantAttack* const found = tyrantAttackOf(attack);
  if (found == nullptr) {
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

/** @returns It steering towards @p column, moving up or down by
 * @p verticalShare of its top speed. */
Intent towardsColumn(const BehaviourView& view, float column,
                     float verticalShare) {
  return Intent{
      .direction = Direction{.x = horizontalShareTowards(column, view),
                             .y = verticalShare},
      .steered = true};
}

/** @returns It keeping its place in an action: its column of the moment, on
 * the row it stands on, since its attacks start from where it is. */
Intent holdingAt(const BehaviourView& view) {
  return towardsColumn(view, columnAt(view), 0.0F);
}

/** @returns Whether @p attack lays its bodies out on the playfield, so it
 * starts from the row it arrived on. */
bool laysOut(std::uint8_t attack) {
  return std::ranges::find(TYRANT_LAID_OUT_ATTACKS, attack) !=
         TYRANT_LAID_OUT_ATTACKS.end();
}

/** @returns A share from 0 to 1 drawn from @p seed, the same for the same
 * seed. */
float shareDrawnFrom(std::uint32_t seed) {
  const std::uint32_t mixed = seed * TYRANT_DRIFT_DRAW_MULTIPLIER;
  return static_cast<float>(mixed >> TYRANT_DRIFT_DRAW_SHIFT) /
         static_cast<float>(1U << TYRANT_DRIFT_DRAW_SHIFT);
}

/** @returns The row it heads for while resting before its attack
 * @p next: the row it arrived on for its arrival and the attacks laid out on
 * the playfield; else, after an even count of actions the row of the nearest
 * ship and after an odd one a row drawn from that count, within
 * TYRANT_DRIFT_SPAN of the row it arrived on. */
float driftRowOf(const BehaviourView& view, std::uint8_t next) {
  const float home = view.origin.y;
  if (view.actionsEnded == 0 || laysOut(next)) {
    return home;
  }
  const bool followsAShip = view.actionsEnded % TYRANT_DRIFT_TURNS == 0 &&
                            view.nearestShip.has_value();
  const float row =
      followsAShip
          ? view.nearestShip->y
          : home + (TYRANT_DRIFT_SPAN *
                    ((TYRANT_DRIFT_SIDES * shareDrawnFrom(view.actionsEnded)) -
                     1.0F));
  return std::clamp(row, home - TYRANT_DRIFT_SPAN, home + TYRANT_DRIFT_SPAN);
}

/** @returns It resting before its attack @p next: its column of the moment,
 * drifting towards driftRowOf() at TYRANT_DRIFT_SPEED at most. */
Intent driftingAt(const BehaviourView& view, std::uint8_t next) {
  const float cap =
      view.topSpeed > 0.0F ? TYRANT_DRIFT_SPEED / view.topSpeed : 0.0F;
  const float share =
      std::clamp(verticalShareTowards(driftRowOf(view, next), view), -cap, cap);
  return towardsColumn(view, columnAt(view), share);
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

/** @returns Its charge at this tick: towards its charge column; once that
 * column came back on the right while it is still left of the middle, on to
 * past the left edge until the whole body left the screen, then a leap to
 * that column: its body is off the screen when it leaps. */
Intent charging(const BehaviourView& view, const ActionTimings& charge) {
  const float column = chargeColumnAt(view, charge);
  if (column > TYRANT_PLAYFIELD_MIDDLE_COLUMN &&
      view.position.x < TYRANT_PLAYFIELD_MIDDLE_COLUMN) {
    if (view.position.x > TYRANT_HIDDEN_LEFT_COLUMN) {
      return towardsColumn(view, TYRANT_CHARGE_LEFT_COLUMN, 0.0F);
    }
    return Intent{.steered = true,
                  .jumpsTo = Position{.x = column, .y = view.position.y}};
  }
  return towardsColumn(view, column, 0.0F);
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

/** @returns What it does while idle: arrive until it ended an action, then
 * drift while it rests, and, rested, its tentacles at a close ship or its
 * next attack. */
Intent whenIdle(const BehaviourView& view) {
  const Intent holding = driftingAt(view, nextAttackOf(view));
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
