#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @brief One explosion of the effect sheet in a burst: how far off the
 * center of the burst, how long after it, and whether it is a fireball of a
 * ship rather than the burst of a Bydo. */
struct IntroBlast {
  sf::Vector2f offset;
  float delay{};
  bool fireball{};
};

}  // namespace rtype::client
