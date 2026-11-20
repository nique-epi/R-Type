#pragma once

#include "LightBeam.hpp"

namespace rtype::client {

/** @brief Two beams that sweep a room in step, mirrored: `left` at its rest
 * angle, turned by the swing, and `right` at its rest angle, turned the other
 * way; the swing goes `swing` radians either side, over `period` seconds
 * there and back. */
struct BeamPair {
  LightBeam left;
  LightBeam right;
  float swing{};
  float period{};
};

}  // namespace rtype::client
