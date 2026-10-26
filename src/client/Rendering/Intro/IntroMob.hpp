#pragma once

#include <SFML/System/Vector2.hpp>
#include "IntroMobKind.hpp"

namespace rtype::client {

/** @brief A Bydo of the intro at a moment of the scene: where its center is,
 * which sprite it shows, and whether the missile about to hit it flashes
 * it. */
struct IntroMob {
  sf::Vector2f center;
  IntroMobKind kind{};
  bool flashed{};
};

}  // namespace rtype::client
