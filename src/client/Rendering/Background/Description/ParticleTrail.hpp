#pragma once

#include <vector>
#include "TrailStop.hpp"

namespace rtype::client {

/**
 * @brief The trail a particle leaves along its path: it grows up to its
 * longest while the particle lives, then shrinks, and the particle is gone
 * once it is.
 */
struct ParticleTrail {
  /** In background pixels. */
  float longest{};
  /** In background pixels per second. */
  float growth{};
  float shrink{};
  /** Its colors, from the particle to its end. */
  std::vector<TrailStop> stops;
};

}  // namespace rtype::client
