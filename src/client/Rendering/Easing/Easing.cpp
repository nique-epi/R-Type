#include "Easing.hpp"
#include <algorithm>
#include <chrono>
#include "RenderingConstants.hpp"
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

float easeInCubic(float progress) {
  const float clamped = std::clamp(progress, 0.0F, 1.0F);
  return clamped * clamped * clamped;
}

float easeInOutCubic(float progress) {
  const float clamped = std::clamp(progress, 0.0F, 1.0F);
  if (clamped < HALF) {
    const float doubled = clamped + clamped;
    return doubled * doubled * doubled * HALF;
  }
  const float remaining = (1.0F - clamped) + (1.0F - clamped);
  return 1.0F - (remaining * remaining * remaining * HALF);
}

float easeOutBack(float progress) {
  const float shifted = std::clamp(progress, 0.0F, 1.0F) - 1.0F;
  const float cube = shifted * shifted * shifted;
  return 1.0F + cube + cube + (shifted * shifted);
}

float progressThrough(float time, float start, float length) {
  if (length <= 0.0F) {
    return time >= start ? 1.0F : 0.0F;
  }
  return std::clamp((time - start) / length, 0.0F, 1.0F);
}

}  // namespace rtype::client
