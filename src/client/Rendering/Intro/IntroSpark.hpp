#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @brief A spark or a drop of a burst: its speed, in pixels of the world a
 * second, which it loses as it ages; how long it lives; how long after the
 * burst it starts; its size, in pixels of the world. */
struct IntroSpark {
  sf::Vector2f velocity;
  float life{};
  float delay{};
  int size{};
  bool operator==(const IntroSpark&) const = default;
};

}  // namespace rtype::client
