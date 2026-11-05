#include "StraightBehaviour.hpp"
#include <string_view>
#include "BehaviourConstants.hpp"
#include "BehaviourView.hpp"
#include "Direction.hpp"
#include "Intent.hpp"

namespace rtype::game {

std::string_view StraightBehaviour::name() const {
  return STRAIGHT_BEHAVIOUR_NAME;
}

Intent StraightBehaviour::steer(const BehaviourView& /*view*/) const {
  return Intent{.direction = Direction{.x = LEFTWARD_STEERING, .y = 0.0F},
                .shooting = true,
                .steered = true};
}

}  // namespace rtype::game
