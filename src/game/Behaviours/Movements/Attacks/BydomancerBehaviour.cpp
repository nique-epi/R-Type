#include "BydomancerBehaviour.hpp"
#include <cmath>
#include <span>
#include <string_view>
#include "ActionCycle.hpp"
#include "ActionKey.hpp"
#include "ActionTimings.hpp"
#include "BehaviourConstants.hpp"
#include "BehaviourSetting.hpp"
#include "BehaviourSteering.hpp"
#include "BehaviourView.hpp"
#include "BydomancerBehaviourConstants.hpp"
#include "Direction.hpp"
#include "EnemyPhase.hpp"
#include "Intent.hpp"
#include "Position.hpp"
#include "SineOffset.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

namespace {

/** @returns The orb, timed by the settings of @p view. */
ActionTimings orbOf(const BehaviourView& view) {
  return timingsAt(view, ORB_ATTACK, BYDOMANCER_SIGNAL_SETTING);
}

/** @returns The nova, a strike of `nova_ms` only. */
ActionTimings novaOf(const BehaviourView& view) {
  return ActionTimings{.attack = NOVA_ATTACK,
                       .signal = engine::Duration::zero(),
                       .strike = millisecondsSettingAt(view, NOVA_SETTING),
                       .recover = engine::Duration::zero()};
}

/** @returns The row the body of @p view keeps: the one it appeared on, or,
 * once wounded, the one `jump` units towards the middle of the area of
 * play. */
float rowOf(const BehaviourView& view) {
  if (!view.wounded) {
    return view.origin.y;
  }
  const auto jump = static_cast<float>(settingAt(view, JUMP_SETTING));
  return view.origin.y < MIDDLE_OF_THE_PLAY_AREA ? view.origin.y + jump
                                                 : view.origin.y - jump;
}

/** @returns Where the body of @p view floats: to its column, then bobbing
 * around @p row. */
Direction floatingOf(const BehaviourView& view, float row) {
  const auto standX = static_cast<float>(settingAt(view, STAND_X_SETTING));
  const auto bob = static_cast<float>(settingAt(view, BOB_SETTING));
  const float height =
      row + sineOffset(view.age, bob,
                       millisecondsSettingAt(view, BOB_PERIOD_SETTING));
  return Direction{.x = view.position.x > standX
                            ? horizontalShareTowards(standX, view)
                            : 0.0F,
                   .y = verticalShareTowards(height, view)};
}

/** @returns Whether a ship is closer to the body of @p view than
 * `nova_reach`. */
bool isAShipClose(const BehaviourView& view) {
  if (!view.nearestShip.has_value()) {
    return false;
  }
  const auto reach = static_cast<float>(settingAt(view, NOVA_REACH_SETTING));
  return std::hypot(view.nearestShip->x - view.position.x,
                    view.nearestShip->y - view.position.y) < reach;
}

/** @returns @p floating with the next step of @p action, the attack the body
 * of @p view is in; it wants to fire during the strike of its orb. */
Intent inAction(const BehaviourView& view, ActionKey action, Intent floating) {
  if (action.attack == NOVA_ATTACK) {
    return withNextPhase(floating, view, novaOf(view));
  }
  floating.shooting = action.phase == EnemyPhase::Strike;
  return withNextPhase(floating, view, orbOf(view));
}

/** @returns @p floating with the attack the idle body of @p view starts, if
 * any: its nova when a ship is close, its orb once rested at its column. */
Intent whenIdle(const BehaviourView& view, Intent floating) {
  const ActionTimings nova = novaOf(view);
  if (isAShipClose(view) &&
      hasRested(view, millisecondsSettingAt(view, NOVA_REST_SETTING))) {
    return withStart(floating, nova);
  }
  const ActionTimings orb = orbOf(view);
  const auto standX = static_cast<float>(settingAt(view, STAND_X_SETTING));
  if (view.position.x <= standX &&
      hasRested(view, millisecondsSettingAt(view, BYDOMANCER_REST_SETTING))) {
    return withStart(floating, orb);
  }
  return floating;
}

}  // namespace

std::string_view BydomancerBehaviour::name() const {
  return BYDOMANCER_BEHAVIOUR_NAME;
}

std::span<const BehaviourSetting> BydomancerBehaviour::settings() const {
  return BYDOMANCER_SETTINGS;
}

Intent BydomancerBehaviour::steer(const BehaviourView& view) const {
  const float row = rowOf(view);
  const auto jump = static_cast<float>(settingAt(view, JUMP_SETTING));
  if (view.wounded &&
      std::abs(view.position.y - row) > jump * LEAP_TOLERANCE_SHARE) {
    return Intent{.steered = true,
                  .jumpsTo = Position{.x = view.position.x, .y = row}};
  }
  const Intent floating{.direction = floatingOf(view, row), .steered = true};
  if (view.action.has_value()) {
    return inAction(view, *view.action, floating);
  }
  return whenIdle(view, floating);
}

}  // namespace rtype::game
