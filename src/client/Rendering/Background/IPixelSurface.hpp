#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/**
 * @brief Where the sky paints, one background pixel at a time. It knows
 * nothing about stars or planets.
 */
class IPixelSurface {
 public:
  virtual ~IPixelSurface() = default;

  /**
   * @brief Paints a pixel over whatever was painted there before.
   *
   * @param pixel Column and row, in background pixels. A pixel outside the
   * background does not show.
   */
  virtual void plot(sf::Vector2i pixel, sf::Color color) = 0;
};

}  // namespace rtype::client
