#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/**
 * @brief The sky behind every screen, as a screen may change it: faster
 * stars, a camera that shakes. ScrollingBackground in the client, a double in
 * the tests.
 */
class ISky {
 public:
  virtual ~ISky() = default;

  /**
   * @brief From the next advance, the stars move @p factor times their own
   * speed, 1 being their speed, 0 at least; faster than
   * STREAK_SPEED_FACTOR, each is drawn with the streak it covers in one
   * frame. The planet and the sky events keep their speed.
   */
  virtual void setSpeedFactor(float factor) = 0;

  /**
   * @brief From the next draw, the stars, the planet and the sky events are
   * drawn @p pixels pixels of the canvas away from their place; the empty sky
   * still covers the whole playfield. (0, 0) puts them back.
   */
  virtual void setOffset(sf::Vector2i pixels) = 0;
};

}  // namespace rtype::client
