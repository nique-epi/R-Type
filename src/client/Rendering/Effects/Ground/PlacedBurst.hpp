#pragma once

#include <SFML/Graphics/Color.hpp>
#include <span>
#include "Burst.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief A burst of particles: its shape, where it bursts and where it
 * aims, the colors its particles go through, and how long it waits. */
struct PlacedBurst {
  Burst shape;
  game::Position center{};
  float aim{};
  std::span<const sf::Color> colors;
  engine::Duration delay{};
};

}  // namespace rtype::client
