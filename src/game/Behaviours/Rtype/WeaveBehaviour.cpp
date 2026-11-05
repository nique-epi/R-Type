#include "WeaveBehaviour.hpp"
#include <chrono>
#include <cmath>
#include <string_view>
#include "BehaviourConstants.hpp"
#include "BehaviourSteering.hpp"
#include "BehaviourView.hpp"
#include "Direction.hpp"
#include "Intent.hpp"
#include "WeaveBehaviourConstants.hpp"

namespace rtype::game {

namespace {

/** @returns Where the weave puts the center of the body of @p view at the end
 * of this tick, vertically. */
float weaveHeight(const BehaviourView& view) {
  const float ageSeconds = std::chrono::duration<float>(view.age).count();
  return view.origin.y +
         (WAVE_AMPLITUDE *
          std::sin(FULL_TURN_RADIANS * ageSeconds / WAVE_PERIOD_SECONDS));
}

}  // namespace

std::string_view WeaveBehaviour::name() const { return WEAVE_BEHAVIOUR_NAME; }

Intent WeaveBehaviour::steer(const BehaviourView& view) const {
  return Intent{.direction = Direction{.x = LEFTWARD_STEERING,
                                       .y = verticalShareTowards(
                                           weaveHeight(view), view)},
                .shooting = true,
                .steered = true};
}

}  // namespace rtype::game
