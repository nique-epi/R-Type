#include "WeaveBehaviour.hpp"
#include <chrono>
#include <cmath>
#include <span>
#include <string_view>
#include "BehaviourConstants.hpp"
#include "BehaviourSetting.hpp"
#include "BehaviourSteering.hpp"
#include "BehaviourView.hpp"
#include "Direction.hpp"
#include "Intent.hpp"
#include "WeaveBehaviourConstants.hpp"

namespace rtype::game {

namespace {

/** @returns Where the weave puts the center of the body of @p view at the end
 * of this tick, vertically, from the amplitude and the period of its
 * settings. */
float weaveHeight(const BehaviourView& view) {
  const float ageSeconds = std::chrono::duration<float>(view.age).count();
  const auto amplitude = static_cast<float>(settingAt(view, AMPLITUDE_SETTING));
  const float periodSeconds =
      std::chrono::duration<float>(
          std::chrono::milliseconds{settingAt(view, PERIOD_SETTING)})
          .count();
  return view.origin.y +
         (amplitude * std::sin(FULL_TURN_RADIANS * ageSeconds / periodSeconds));
}

}  // namespace

std::string_view WeaveBehaviour::name() const { return WEAVE_BEHAVIOUR_NAME; }

std::span<const BehaviourSetting> WeaveBehaviour::settings() const {
  return WEAVE_SETTINGS;
}

Intent WeaveBehaviour::steer(const BehaviourView& view) const {
  return Intent{.direction = Direction{.x = LEFTWARD_STEERING,
                                       .y = verticalShareTowards(
                                           weaveHeight(view), view)},
                .shooting = true,
                .steered = true};
}

}  // namespace rtype::game
