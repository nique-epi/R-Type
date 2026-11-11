#pragma once

#include <SFML/System/Vector2.hpp>
#include "SkyLevel.hpp"

namespace rtype::client {

/**
 * @brief The sky behind every screen, as a screen may change it: faster
 * stars, a camera that shakes, the decor of the biome a level shows.
 * ScrollingBackground in the client, a double in the tests.
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

  /**
   * @brief From the next draw, the decor of the biome of @p level is drawn
   * over the stars, where the scroll of @p level puts it, and from the next
   * advance it moves at the speed of that scroll. A biome other than the one
   * shown fades in while the one shown fades out. A level without a biome,
   * or with a biome the client has no decor for, shows the stars alone.
   */
  virtual void showLevel(const SkyLevel& level) = 0;
};

}  // namespace rtype::client
