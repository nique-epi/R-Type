#include "DrillbackBehaviour.hpp"
#include <span>
#include <string_view>
#include "ActionCycle.hpp"
#include "ActionTimings.hpp"
#include "BehaviourConstants.hpp"
#include "BehaviourSetting.hpp"
#include "BehaviourSteering.hpp"
#include "BehaviourView.hpp"
#include "Direction.hpp"
#include "DrillbackBehaviourConstants.hpp"
#include "EnemyPhase.hpp"
#include "Intent.hpp"

namespace rtype::game {

namespace {

/** @returns The charge, timed by the settings of @p view. */
ActionTimings chargeOf(const BehaviourView& view) {
  return ActionTimings{
      .attack = CHARGE_ATTACK,
      .signal = millisecondsSettingAt(view, DRILLBACK_SIGNAL_SETTING),
      .strike = millisecondsSettingAt(view, DRILLBACK_STRIKE_SETTING),
      .recover = millisecondsSettingAt(view, DRILLBACK_RECOVER_SETTING)};
}

/** @returns The way the body of @p view drifts with the sky: to the left at
 * `drift_speed`, as a share of its top speed; nowhere without a top speed. */
Direction driftOf(const BehaviourView& view) {
  if (view.topSpeed <= 0.0F) {
    return Direction{.x = 0.0F, .y = 0.0F};
  }
  const auto drift = static_cast<float>(settingAt(view, DRIFT_SPEED_SETTING));
  return Direction{.x = LEFTWARD_STEERING * drift / view.topSpeed, .y = 0.0F};
}

/** @returns Whether the body of @p view charges at this tick. */
bool isCharging(const BehaviourView& view) {
  return view.action.has_value() && view.action->phase == EnemyPhase::Strike;
}

}  // namespace

std::string_view DrillbackBehaviour::name() const {
  return DRILLBACK_BEHAVIOUR_NAME;
}

std::span<const BehaviourSetting> DrillbackBehaviour::settings() const {
  return DRILLBACK_SETTINGS;
}

Intent DrillbackBehaviour::steer(const BehaviourView& view) const {
  const ActionTimings charge = chargeOf(view);
  const Direction way = isCharging(view)
                            ? Direction{.x = LEFTWARD_STEERING, .y = 0.0F}
                            : driftOf(view);
  const Intent moving{.direction = way, .steered = true};
  if (view.action.has_value()) {
    return withNextPhase(moving, view, charge);
  }
  const auto enterX = static_cast<float>(settingAt(view, ENTER_X_SETTING));
  if (view.position.x <= enterX &&
      hasRested(view, charge,
                millisecondsSettingAt(view, DRILLBACK_REST_SETTING))) {
    return withStart(moving, charge);
  }
  return moving;
}

}  // namespace rtype::game
