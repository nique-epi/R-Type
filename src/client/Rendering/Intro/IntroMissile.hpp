#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/**
 * @brief A missile of the intro, between two moments of the scene: it flies
 * from @c from to @c to, which it always hits, swerving @c curve pixels off
 * its line at mid-flight (0 flies straight). A missile of an enemy shows no
 * impact.
 */
struct IntroMissile {
  sf::Vector2f from;
  sf::Vector2f to;
  float start{};
  float end{};
  float curve{};
  bool enemy{};
};

}  // namespace rtype::client
