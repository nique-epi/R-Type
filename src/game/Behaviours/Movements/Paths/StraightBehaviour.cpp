#include "StraightBehaviour.hpp"
#include <span>
#include <string_view>
#include "BehaviourConstants.hpp"
#include "BehaviourSetting.hpp"
#include "BehaviourSteering.hpp"
#include "BehaviourView.hpp"
#include "Direction.hpp"
#include "DirectionConstants.hpp"
#include "Intent.hpp"
#include "StraightBehaviourConstants.hpp"

namespace rtype::game {

std::string_view StraightBehaviour::name() const {
  return STRAIGHT_BEHAVIOUR_NAME;
}

std::span<const BehaviourSetting> StraightBehaviour::settings() const {
  return STRAIGHT_SETTINGS;
}

Intent StraightBehaviour::steer(const BehaviourView& view) const {
  const auto heading = static_cast<float>(settingAt(view, HEADING_SETTING));
  return Intent{.direction = Direction{.x = heading * FULL_STEERING, .y = 0.0F},
                .shooting = true,
                .steered = true};
}

}  // namespace rtype::game
