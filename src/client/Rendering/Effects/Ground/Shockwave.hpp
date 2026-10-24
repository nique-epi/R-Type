#pragma once

#include <SFML/Graphics/Color.hpp>
#include <span>
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Component of a shockwave centered on the Position of its entity: a
 * circle one pixel thick that opens to `radius` pixels of the canvas over its
 * life, through `colors`, hottest first. A negative age keeps it hidden. The
 * colors are only viewed, like those of a Particle.
 */
struct Shockwave {
  int radius{};
  engine::Duration age{};
  engine::Duration life{};
  std::span<const sf::Color> colors;
};

}  // namespace rtype::client
