#include "ReaperBehaviour.hpp"
#include <cmath>
#include <span>
#include <string_view>
#include "ActionCycle.hpp"
#include "ActionTimings.hpp"
#include "BehaviourConstants.hpp"
#include "BehaviourSetting.hpp"
#include "BehaviourSteering.hpp"
#include "BehaviourView.hpp"
#include "Direction.hpp"
#include "Intent.hpp"
#include "Position.hpp"
#include "ReaperBehaviourConstants.hpp"

namespace rtype::game {

namespace {

/** @returns The swing of the scythe, timed by the settings of @p view. */
ActionTimings scytheOf(const BehaviourView& view) {
  return ActionTimings{
      .attack = SCYTHE_ATTACK,
      .signal = millisecondsSettingAt(view, REAPER_SIGNAL_SETTING),
      .strike = millisecondsSettingAt(view, REAPER_STRIKE_SETTING),
      .recover = millisecondsSettingAt(view, REAPER_RECOVER_SETTING)};
}

/** @returns Where the reaper of @p view swings from at @p ship: `reach` to its
 * right, at its height. */
Position strikingPointAt(const BehaviourView& view, const Position& ship) {
  return Position{
      .x = ship.x + static_cast<float>(settingAt(view, REACH_SETTING)),
      .y = ship.y};
}

/** @returns Whether the body of @p view is within `align` of @p point on each
 * axis. */
bool isAt(const BehaviourView& view, const Position& point) {
  const auto align = static_cast<float>(settingAt(view, ALIGN_SETTING));
  return std::abs(view.position.x - point.x) <= align &&
         std::abs(view.position.y - point.y) <= align;
}

}  // namespace

std::string_view ReaperBehaviour::name() const { return REAPER_BEHAVIOUR_NAME; }

std::span<const BehaviourSetting> ReaperBehaviour::settings() const {
  return REAPER_SETTINGS;
}

Intent ReaperBehaviour::steer(const BehaviourView& view) const {
  const ActionTimings scythe = scytheOf(view);
  if (view.action.has_value()) {
    return withNextPhase(Intent{.steered = true}, view, scythe);
  }
  if (!view.nearestShip.has_value()) {
    return Intent{.direction = Direction{.x = LEFTWARD_STEERING, .y = 0.0F},
                  .steered = true};
  }
  const Position point = strikingPointAt(view, *view.nearestShip);
  if (isAt(view, point) &&
      hasRested(view, scythe,
                millisecondsSettingAt(view, REAPER_REST_SETTING))) {
    return withStart(Intent{.steered = true}, scythe);
  }
  return Intent{
      .direction = Direction{.x = horizontalShareTowards(point.x, view),
                             .y = verticalShareTowards(point.y, view)},
      .steered = true};
}

}  // namespace rtype::game
