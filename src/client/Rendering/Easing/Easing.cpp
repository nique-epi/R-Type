#include "Easing.hpp"
#include <algorithm>
#include <chrono>
#include "TimeConstants.hpp"

namespace rtype::client {

float easeOutQuad(float progress) {
  const float remaining = 1.0F - std::clamp(progress, 0.0F, 1.0F);
  return 1.0F - (remaining * remaining);
}

float easeOutCubic(float progress) {
  const float remaining = 1.0F - std::clamp(progress, 0.0F, 1.0F);
  return 1.0F - (remaining * remaining * remaining);
}

float shareOf(engine::Duration part, engine::Duration whole) {
  return std::chrono::duration<float>(part).count() /
         std::chrono::duration<float>(whole).count();
}

}  // namespace rtype::client
