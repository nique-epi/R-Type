#include "WeaveBehaviour.hpp"
#include <span>
#include <string_view>
#include "BehaviourConstants.hpp"
#include "BehaviourSetting.hpp"
#include "BehaviourSteering.hpp"
#include "BehaviourView.hpp"
#include "Direction.hpp"
#include "Intent.hpp"
#include "SineOffset.hpp"
#include "WeaveBehaviourConstants.hpp"

namespace rtype::game {

namespace {

/** @returns Where the weave puts the center of the body of @p view at the end
 * of this tick, vertically, from the amplitude and the period of its
 * settings. */
float weaveHeight(const BehaviourView& view) {
  const auto amplitude = static_cast<float>(settingAt(view, AMPLITUDE_SETTING));
  return view.origin.y +
         sineOffset(view.age, amplitude,
                    millisecondsSettingAt(view, PERIOD_SETTING));
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
