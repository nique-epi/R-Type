#include <gtest/gtest.h>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <optional>
#include <set>
#include <utility>
#include "BackgroundColors.hpp"
#include "BackgroundConstants.hpp"
#include "ColoredPixel.hpp"
#include "PlanetImage.hpp"

using rtype::client::ColoredPixel;
using rtype::client::PLANET_CENTER;
using rtype::client::PLANET_IMAGE_SIZE;
using rtype::client::PLANET_RADIUS;
using rtype::client::PLANET_RING_BACK_COLOR;
using rtype::client::PLANET_RING_FRONT_COLOR;
using rtype::client::PLANET_RING_HEIGHT_RADII;
using rtype::client::PLANET_SHADES;
using rtype::client::PlanetImage;

namespace {

std::optional<sf::Color> colorAt(const PlanetImage& planet,
                                 sf::Vector2i pixel) {
  const auto found =
      std::ranges::find(planet.pixels(), pixel, &ColoredPixel::position);
  if (found == planet.pixels().end()) {
    return std::nullopt;
  }
  return found->color;
}

bool isOnSphere(sf::Vector2i pixel) {
  return (pixel - PLANET_CENTER).lengthSquared() <=
         PLANET_RADIUS * PLANET_RADIUS;
}

/** @returns The index of the color in PLANET_SHADES, when it is a shade. */
std::optional<std::size_t> shadeIndexOf(sf::Color color) {
  for (std::size_t shade = 0; shade < PLANET_SHADES.size(); ++shade) {
    if (PLANET_SHADES[shade] == color) {
      return shade;
    }
  }
  return std::nullopt;
}

/** @returns The mean shade index of the sphere pixels a filter keeps. */
template <typename Filter>
double meanShade(const PlanetImage& planet, Filter keeps) {
  double total = 0.0;
  std::size_t count = 0;
  for (const ColoredPixel& pixel : planet.pixels()) {
    const std::optional<std::size_t> shade = shadeIndexOf(pixel.color);
    if (!shade.has_value() || !keeps(pixel.position - PLANET_CENTER)) {
      continue;
    }
    total += static_cast<double>(*shade);
    ++count;
  }
  return count == 0 ? 0.0 : total / static_cast<double>(count);
}

}  // namespace

/**
 * Given the planet image
 * When its pixels are listed
 * Then every pixel lies inside the image
 */
TEST(PlanetImage, EveryPixelLiesInsideTheImage) {
  const PlanetImage planet;

  for (const ColoredPixel& pixel : planet.pixels()) {
    EXPECT_GE(pixel.position.x, 0);
    EXPECT_GE(pixel.position.y, 0);
    EXPECT_LT(pixel.position.x, static_cast<int>(PLANET_IMAGE_SIZE.x));
    EXPECT_LT(pixel.position.y, static_cast<int>(PLANET_IMAGE_SIZE.y));
  }
}

/**
 * Given the planet image
 * When its pixels are listed
 * Then no pixel appears twice
 */
TEST(PlanetImage, EachPixelAppearsOnce) {
  const PlanetImage planet;
  std::set<std::pair<int, int>> positions;

  for (const ColoredPixel& pixel : planet.pixels()) {
    positions.emplace(pixel.position.x, pixel.position.y);
  }

  EXPECT_EQ(positions.size(), planet.pixels().size());
}

/**
 * Given the planet image
 * When the shades of its top-left and bottom-right quarters are compared
 * Then the top-left quarter is lighter: the light comes from the top-left
 */
TEST(PlanetImage, SphereIsLitFromTheTopLeft) {
  const PlanetImage planet;

  const double topLeft = meanShade(
      planet, [](sf::Vector2i offset) { return offset.x < 0 && offset.y < 0; });
  const double bottomRight = meanShade(
      planet, [](sf::Vector2i offset) { return offset.x > 0 && offset.y > 0; });

  EXPECT_GT(topLeft, bottomRight);
}

/**
 * Given the planet image
 * When the pixel straight below the center, at the lowest point of the ring,
 * is read
 * Then it has the color of the ring in front: the ring passes over the sphere
 */
TEST(PlanetImage, FrontOfTheRingCrossesTheSphere) {
  const PlanetImage planet;
  const sf::Vector2i lowestRingPoint =
      PLANET_CENTER +
      sf::Vector2i(
          0, static_cast<int>(std::lround(static_cast<float>(PLANET_RADIUS) *
                                          PLANET_RING_HEIGHT_RADII)));

  EXPECT_EQ(colorAt(planet, lowestRingPoint), PLANET_RING_FRONT_COLOR);
}

/**
 * Given the planet image
 * When the pixels of the back half of the ring are located
 * Then none of them lies on the sphere: the sphere hides the ring behind it
 */
TEST(PlanetImage, SphereHidesTheBackOfTheRing) {
  const PlanetImage planet;
  std::size_t backPixels = 0;

  for (const ColoredPixel& pixel : planet.pixels()) {
    if (pixel.color == PLANET_RING_BACK_COLOR) {
      ++backPixels;
      EXPECT_FALSE(isOnSphere(pixel.position));
    }
  }

  EXPECT_GT(backPixels, 0U);
}
