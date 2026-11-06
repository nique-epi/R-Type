#pragma once

namespace rtype::client {

/** @brief Where a server stands from the planet: how far, by its ping, and in
 * which direction, from -MAXIMUM_ANGLE_DEGREES (up) to MAXIMUM_ANGLE_DEGREES
 * (down). */
struct StarBearing {
  double pingMilliseconds{};
  int angleDegrees{};
};

}  // namespace rtype::client
