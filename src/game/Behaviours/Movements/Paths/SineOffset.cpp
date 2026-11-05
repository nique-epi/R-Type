#include "SineOffset.hpp"
#include <chrono>
#include <cmath>
#include "BehaviourConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

float sineOffset(engine::Duration age, float amplitude,
                 engine::Duration period) {
  const float turns =
      std::chrono::duration<float>(age) / std::chrono::duration<float>(period);
  return amplitude * std::sin(FULL_TURN_RADIANS * turns);
}

}  // namespace rtype::game
