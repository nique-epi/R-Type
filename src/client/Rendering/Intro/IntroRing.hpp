#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @brief A ring one pixel wide that grows from a point to @c radius pixels
 * of the world, from @c start for @c life seconds of the scene. */
struct IntroRing {
  sf::Vector2f center;
  float start{};
  float radius{};
  float life{};
};

}  // namespace rtype::client
