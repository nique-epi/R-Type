#pragma once

#include <SFML/Graphics/Color.hpp>
#include <span>
#include "Position.hpp"
#include "Ring.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief A shockwave: its shape, its center, its colors, and how long it
 * waits. */
struct PlacedRing {
  Ring shape;
  game::Position center{};
  std::span<const sf::Color> colors;
  engine::Duration delay{};
};

}  // namespace rtype::client
