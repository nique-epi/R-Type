#pragma once

#include <SFML/System/Vector2.hpp>
#include "LayerParticle.hpp"

namespace rtype::client {

/** @brief A particle as it is painted at a moment: where it lies on the
 * background, the streak it leaves while its layer warps, and whether it
 * twinkles now. */
struct PaintedParticle {
  LayerParticle particle;
  sf::Vector2f place;
  int streak{};
  bool twinkling{};
};

}  // namespace rtype::client
