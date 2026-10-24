#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <span>
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Component of one pixel of debris, of a drop or of a spark: its speed,
 * in logical units per second, its age and its life, and the colors it goes
 * through as it ages, hottest first. GroundEffectSystem moves it, slows it
 * down and destroys it once its life is spent; paintGroundEffects() paints it
 * two pixels by two while it is young and `shrinks`, one pixel otherwise. A
 * negative age keeps it hidden and still: a burst that waits for a hold. The
 * colors are only viewed: the array they come from must outlive it, which a
 * constant array does.
 */
struct Particle {
  sf::Vector2f velocity;
  engine::Duration age{};
  engine::Duration life{};
  std::span<const sf::Color> colors;
  bool shrinks = false;
};

}  // namespace rtype::client
