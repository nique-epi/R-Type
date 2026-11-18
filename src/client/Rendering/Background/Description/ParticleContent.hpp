#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "BackgroundFileConstants.hpp"
#include "DecimalRange.hpp"
#include "DurationRange.hpp"
#include "ParticleSprite.hpp"
#include "ParticleTrail.hpp"

namespace rtype::client {

/**
 * @brief A layer of particles, each a pixel or a sprite.
 *
 * It is either a population, as many particles as it says, always there,
 * which only move with their layer and come back at a new height each time it
 * repeats (stars); or the particles of an emission group, born one at a time
 * when the group draws this layer, which fly on their own and end (a shooting
 * star, an asteroid).
 */
struct ParticleContent {
  /** How many particles a population holds; 0 for the particles of a
   * group. */
  std::size_t population{};
  /** The name of the emission group that gives birth to the particles; empty
   * for a population. */
  std::string group;
  /** The chance of this layer among the layers of its group. */
  double weight{DEFAULT_WEIGHT};
  /** Where a particle is born, drawn at random, in background pixels. */
  sf::FloatRect area;
  /** In background pixels per second; 0 for a particle that stays. */
  engine::DecimalRange speed;
  /** In degrees: 0 flies to the right, 90 down, 180 to the left. */
  engine::DecimalRange angle{.lowest = LEFTWARD_ANGLE,
                             .highest = LEFTWARD_ANGLE};
  /** The most a particle moves up or down per second, on top of its speed:
   * drawn between that much up and that much down. */
  float drift{};
  /** How long a particle lives; without it, a particle ends once it is more
   * than margin pixels out of the background. */
  std::optional<DurationRange> life;
  float margin{};
  /** The color of a particle drawn as one pixel; nothing when it is drawn
   * with one of sprites. */
  std::optional<sf::Color> pixel;
  std::vector<ParticleSprite> sprites;
  /** The color a pixel takes while it twinkles. */
  std::optional<sf::Color> twinkle;
  /** A particle twinkles during one twinkle step out of this many; 0 never
   * twinkles. */
  std::int64_t twinkleSteps{};
  std::optional<ParticleTrail> trail;
  /** The color of the streak behind a particle while its layer warps. */
  std::optional<sf::Color> warpTrail;
};

}  // namespace rtype::client
