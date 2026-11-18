#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>

namespace rtype::client {

/** @brief One particle of a layer of particles. */
struct LayerParticle {
  /** Where it lies in its layer, in background pixels. */
  sf::Vector2f position;
  /** In background pixels per second. */
  sf::Vector2f velocity;
  /** How long it has lived, in seconds. */
  float age{};
  /** How long it lives, in seconds; 0 for a particle that lives until it
   * leaves the background, or for good. */
  float life{};
  /** How long its trail is now, in background pixels. */
  float trailLength{};
  /** The rank of the sprite it is drawn with among those of its layer. */
  std::size_t sprite{};
  /** The frame a sprite that loops shows, with its fraction, and the frames
   * it moves on by per second. */
  float frame{};
  float frameRate{};
  /** The twinkle step, in its cycle, a particle of a population twinkles
   * at. */
  std::int64_t twinklePhase{};
  /** How many times a particle of a population came back from the right. */
  int lap{};
};

}  // namespace rtype::client
