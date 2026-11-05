#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @brief Where the planet stands, how large, and how far it and the grains
 * of its ring have turned, in radians. */
struct PlanetPose {
  sf::Vector2i center;
  int radius{};
  double spin{};
  double drift{};
};

}  // namespace rtype::client
