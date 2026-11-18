#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <optional>
#include <string>
#include "BackgroundFileConstants.hpp"
#include "DecimalRange.hpp"

namespace rtype::client {

/**
 * @brief One picture a particle may be drawn with: a strip of frames of the
 * same size, side by side in one image.
 *
 * With framesPerSecond, the frames loop at a rate drawn in that range, from a
 * frame drawn at random. Without, a particle with a life shows them once over
 * its life; a particle that twinkles shows its second frame while it
 * twinkles; any other shows the first.
 */
struct ParticleSprite {
  /** The asset id of the image. */
  std::string file;
  std::size_t frames{DEFAULT_FRAMES};
  /** Its chance among the sprites of its layer. */
  double weight{DEFAULT_WEIGHT};
  /** The pixel of a frame that lies where the particle is. */
  sf::Vector2f anchor;
  std::optional<engine::DecimalRange> framesPerSecond;
};

}  // namespace rtype::client
