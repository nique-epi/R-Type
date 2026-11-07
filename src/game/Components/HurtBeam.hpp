#pragma once

#include "ActionKey.hpp"
#include "HurtWindow.hpp"

namespace rtype::game {

/**
 * @brief A beam of a hurt zone, in logical units: a band `thickness` thick
 * and `length` long from its pivot, (offsetX, offsetY) from the Position of
 * the enemy, heading left turned by its angle, a positive angle turning it
 * down. It hurts in `activeIn`, inside `window`. While its window runs, its
 * angle goes evenly from fromDegrees to toDegrees; a beam whose window has no
 * end keeps fromDegrees, and must have toDegrees equal to it.
 */
struct HurtBeam {
  float offsetX{0.0F};
  float offsetY{0.0F};
  float length{0.0F};
  float thickness{0.0F};
  float fromDegrees{0.0F};
  float toDegrees{0.0F};
  ActionKey activeIn{};
  HurtWindow window{};

  bool operator==(const HurtBeam&) const = default;
};

}  // namespace rtype::game
