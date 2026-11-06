#include "ScatterBehaviour.hpp"
#include <cmath>
#include <span>
#include <string_view>
#include "BehaviourConstants.hpp"
#include "BehaviourSetting.hpp"
#include "BehaviourView.hpp"
#include "Direction.hpp"
#include "Intent.hpp"

namespace rtype::game {

std::string_view ScatterBehaviour::name() const {
  return SCATTER_BEHAVIOUR_NAME;
}

std::span<const BehaviourSetting> ScatterBehaviour::settings() const {
  return {};
}

Intent ScatterBehaviour::steer(const BehaviourView& view) const {
  const float across = view.position.x - view.origin.x;
  const float down = view.position.y - view.origin.y;
  const float length = std::hypot(across, down);
  if (length == 0.0F) {
    return Intent{.direction = LEFTWARD, .steered = true};
  }
  return Intent{
      .direction = Direction{.x = across / length, .y = down / length},
      .steered = true};
}

}  // namespace rtype::game
