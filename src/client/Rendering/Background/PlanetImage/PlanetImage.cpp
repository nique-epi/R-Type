#include "PlanetImage.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>
#include "BackgroundColors.hpp"
#include "BackgroundConstants.hpp"
#include "ColoredPixel.hpp"
#include "PixelRounding.hpp"

namespace rtype::client {

PlanetImage::PlanetImage() {
  sf::Image image(PLANET_IMAGE_SIZE, sf::Color::Transparent);
  drawRing(image, RingHalf::Behind);
  drawSphere(image);
  drawRing(image, RingHalf::InFront);
  for (unsigned int row = 0; row < PLANET_IMAGE_SIZE.y; ++row) {
    for (unsigned int column = 0; column < PLANET_IMAGE_SIZE.x; ++column) {
      const sf::Color color = image.getPixel({column, row});
      if (color != sf::Color::Transparent) {
        pixels_.push_back({.position = sf::Vector2i(sf::Vector2u(column, row)),
                           .color = color});
      }
    }
  }
}

const std::vector<ColoredPixel>& PlanetImage::pixels() const { return pixels_; }

void PlanetImage::drawRing(sf::Image& image, RingHalf half) {
  const auto radius = static_cast<float>(PLANET_RADIUS);
  const sf::Vector2f center(PLANET_CENTER);
  for (int step = 0;
       static_cast<float>(step) * PLANET_RING_ANGLE_STEP < FULL_TURN; ++step) {
    const float angle = static_cast<float>(step) * PLANET_RING_ANGLE_STEP;
    const bool inFront = std::sin(angle) > 0.0F;
    if (inFront != (half == RingHalf::InFront)) {
      continue;
    }
    const sf::Vector2f offset(
        std::cos(angle) * radius * PLANET_RING_WIDTH_RADII,
        (std::sin(angle) * radius * PLANET_RING_HEIGHT_RADII) -
            (std::cos(angle) * PLANET_RING_TILT));
    paintInside(image, nearestPixel(center + offset),
                inFront ? PLANET_RING_FRONT_COLOR : PLANET_RING_BACK_COLOR);
  }
}

void PlanetImage::drawSphere(sf::Image& image) {
  const auto radius = static_cast<float>(PLANET_RADIUS);
  const auto lastShade = PLANET_SHADES.size() - 1;
  for (int row = -PLANET_RADIUS; row <= PLANET_RADIUS; ++row) {
    for (int column = -PLANET_RADIUS; column <= PLANET_RADIUS; ++column) {
      const sf::Vector2f normal =
          sf::Vector2f(sf::Vector2i(column, row)) / radius;
      const float distanceSquared = normal.lengthSquared();
      if (distanceSquared > 1.0F) {
        continue;
      }
      const float light = std::clamp(
          normal.dot(PLANET_LIGHT_ACROSS) +
              (PLANET_LIGHT_FROM_VIEWER * std::sqrt(1.0F - distanceSquared)),
          0.0F, PLANET_BRIGHTEST_LIGHT);
      const auto ditherRow = static_cast<std::size_t>(row + PLANET_RADIUS) %
                             PLANET_DITHER_MATRIX.size();
      const auto ditherColumn =
          static_cast<std::size_t>(column + PLANET_RADIUS) %
          PLANET_DITHER_MATRIX.size();
      const float threshold =
          static_cast<float>(PLANET_DITHER_MATRIX[ditherRow][ditherColumn]) /
          PLANET_DITHER_LEVELS;
      const std::size_t shade = std::min(
          lastShade, static_cast<std::size_t>(
                         (light * static_cast<float>(lastShade)) + threshold));
      paintInside(image, PLANET_CENTER + sf::Vector2i(column, row),
                  PLANET_SHADES[shade]);
    }
  }
}

void PlanetImage::paintInside(sf::Image& image, sf::Vector2i pixel,
                              sf::Color color) {
  const sf::Vector2i size(image.getSize());
  if (pixel.x < 0 || pixel.y < 0 || pixel.x >= size.x || pixel.y >= size.y) {
    return;
  }
  image.setPixel(sf::Vector2u(pixel), color);
}

}  // namespace rtype::client
