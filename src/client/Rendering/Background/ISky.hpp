#pragma once

#include <SFML/System/Vector2.hpp>
#include "SkyLevel.hpp"

namespace rtype::client {

/**
 * @brief The sky behind every screen, as a screen may change it: layers
 * that warp, a camera that shakes, the background of the biome a level
 * shows. Sky in the client, a double in the tests.
 */
class ISky {
 public:
  virtual ~ISky() = default;

  /**
   * @brief From the next advance, the layers that warp, the stars of the
   * sky, move @p factor times their own speed, 1 being their speed, 0 at
   * least; faster than STREAK_SPEED_FACTOR, their particles are drawn with
   * the streak they cover in one tick. The other layers keep their speed.
   */
  virtual void setSpeedFactor(float factor) = 0;

  /**
   * @brief From the next draw, every layer is drawn @p pixels pixels of the
   * canvas away from its place; the empty sky still covers the whole
   * playfield. (0, 0) puts them back.
   */
  virtual void setOffset(sf::Vector2i pixels) = 0;

  /**
   * @brief From the next draw, the sky shows the background named after the
   * biome of @p level, where the scroll of @p level puts its layers, and
   * from the next advance they follow that scroll at its speed. A level
   * without a biome, or with a biome the client has no background for, shows
   * DEFAULT_BACKGROUND_NAME.
   */
  virtual void showLevel(const SkyLevel& level) = 0;
};

}  // namespace rtype::client
