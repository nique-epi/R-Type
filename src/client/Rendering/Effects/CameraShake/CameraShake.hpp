#pragma once

#include <SFML/System/Vector2.hpp>
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The shake of the camera, after Eiserloh (GDC 2016): a trauma from 0
 * to 1 that events raise and time lowers, and an offset of whole pixels of
 * the canvas, without rotation, that follows a smoothed noise.
 *
 * The offset moves the sky and the entities, never the status band: the game
 * screen reads it each frame. It depends on time only, never on the number of
 * frames drawn.
 */
class CameraShake {
 public:
  /** @brief Adds trauma, the total capped at 1; a negative amount adds
   * nothing. */
  void addTrauma(float amount);

  /** @brief Takes TRAUMA_DECAY_PER_SECOND of trauma away per second, never
   * under 0, and moves the noise on. */
  void advance(engine::Duration elapsed);

  /** @returns How many pixels of the canvas the camera is off on each axis:
   * SHAKE_MAXIMUM_OFFSET times the trauma to the power SHAKE_EXPONENT, times
   * the noise of the axis, between -1 and 1, rounded. (0, 0) without trauma.
   */
  [[nodiscard]] sf::Vector2i offset() const;

  [[nodiscard]] float trauma() const;

 private:
  float trauma_{};
  engine::Duration noiseTime_{};
};

}  // namespace rtype::client
