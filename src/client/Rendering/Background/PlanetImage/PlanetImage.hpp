#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <vector>
#include "ColoredPixel.hpp"

namespace rtype::client {

/**
 * @brief Pixels of the ringed planet, computed once.
 *
 * The sphere is lit from the top-left and shaded with four colors mixed by a
 * 2 x 2 Bayer matrix, so neighbouring shades blend into a checkerboard. The
 * half of the ring behind the planet is drawn first, darker, and the sphere
 * hides it where they overlap; the half in front is drawn last, over the
 * sphere.
 */
class PlanetImage {
 public:
  PlanetImage();

  /**
   * @returns Each colored pixel once, relative to the top-left corner of an
   * image PLANET_IMAGE_SIZE large. Uncolored pixels are absent.
   */
  [[nodiscard]] const std::vector<ColoredPixel>& pixels() const;

 private:
  enum class RingHalf : std::uint8_t { Behind, InFront };

  static void drawRing(sf::Image& image, RingHalf half);
  static void drawSphere(sf::Image& image);
  static void paintInside(sf::Image& image, sf::Vector2i pixel,
                          sf::Color color);

  std::vector<ColoredPixel> pixels_;
};

}  // namespace rtype::client
