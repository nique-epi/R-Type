#include "UnitHeading.hpp"
#include <cmath>
#include "BehaviourConstants.hpp"
#include "Direction.hpp"

namespace rtype::game {

Direction headingAlong(Direction offset) {
  const float length = std::hypot(offset.x, offset.y);
  if (length == 0.0F) {
    return LEFTWARD;
  }
  return Direction{.x = offset.x / length, .y = offset.y / length};
}

}  // namespace rtype::game
