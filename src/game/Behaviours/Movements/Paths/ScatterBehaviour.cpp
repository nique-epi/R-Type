#include "ScatterBehaviour.hpp"
#include <span>
#include <string_view>
#include "BehaviourConstants.hpp"
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "Direction.hpp"
#include "Intent.hpp"
#include "UnitHeading.hpp"

namespace rtype::game {

std::string_view ScatterBehaviour::name() const {
  return SCATTER_BEHAVIOUR_NAME;
}

std::span<const BehaviourSetting> ScatterBehaviour::settings() const {
  return {};
}

Intent ScatterBehaviour::steer(const BehaviourView& view) const {
  return Intent{.direction = headingAlong(
                    Direction{.x = view.position.x - view.origin.x,
                              .y = view.position.y - view.origin.y}),
                .shooting = true,
                .steered = true};
}

}  // namespace rtype::game
