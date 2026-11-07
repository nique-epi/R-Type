#include "MetalTexture.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <random>
#include <vector>
#include "BackgroundColorConstants.hpp"
#include "ColoredPixel.hpp"
#include "HangarColorConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "MetalPatch.hpp"
#include "MetalTextureConstants.hpp"
#include "RandomRange.hpp"

namespace rtype::client {

namespace {

/** @returns A whole number drawn from 0 to @p count - 1. */
int drawnBelow(std::mt19937& random, int count) {
  return std::min(count - 1, static_cast<int>(randomBetween(
                                 random, 0.0F, static_cast<float>(count))));
}

/** @returns How many of something a patch gets for @p perPixel per pixel. */
int countFor(sf::IntRect area, float perPixel) {
  return static_cast<int>(static_cast<float>(area.size.x * area.size.y) *
                          perPixel);
}

void addEdgeWear(std::vector<ColoredPixel>& pixels, const MetalPatch& patch,
                 std::mt19937& random) {
  const sf::IntRect area = patch.area;
  for (int column = 0; column < area.size.x; ++column) {
    if (randomChance(random, TOP_EDGE_WEAR_CHANCE)) {
      pixels.push_back({.position = area.position + sf::Vector2i(column, 0),
                        .color = patch.shades.light});
    }
    if (randomChance(random, BOTTOM_EDGE_WEAR_CHANCE)) {
      pixels.push_back(
          {.position = area.position + sf::Vector2i(column, area.size.y - 1),
           .color = patch.shades.light});
    }
  }
}

void addScratches(std::vector<ColoredPixel>& pixels, const MetalPatch& patch,
                  std::mt19937& random) {
  const sf::IntRect area = patch.area;
  for (int scratch = 0; scratch < countFor(area, SCRATCHES_PER_PIXEL);
       ++scratch) {
    const sf::Vector2i start =
        area.position +
        sf::Vector2i(
            drawnBelow(random, std::max(1, area.size.x - LONGEST_SCRATCH)),
            drawnBelow(random, std::max(1, area.size.y - LONGEST_SCRATCH)));
    const int length = 1 + drawnBelow(random, LONGEST_SCRATCH);
    for (int step = 0; step < length; ++step) {
      pixels.push_back({.position = start + sf::Vector2i(step, step / 2),
                        .color = patch.shades.light});
    }
  }
}

void addGrime(std::vector<ColoredPixel>& pixels, const MetalPatch& patch,
              std::mt19937& random) {
  const sf::IntRect area = patch.area;
  for (int speck = 0; speck < countFor(area, GRIME_PER_PIXEL); ++speck) {
    const float upper = randomBetween(random, 0.0F, 1.0F);
    const float down = 1.0F - (upper * randomBetween(random, 0.0F, 1.0F));
    pixels.push_back(
        {.position = area.position +
                     sf::Vector2i(
                         drawnBelow(random, area.size.x),
                         std::min(area.size.y - 1,
                                  static_cast<int>(
                                      down * static_cast<float>(area.size.y)))),
         .color = patch.shades.dark});
  }
}

}  // namespace

void addBrushStrokes(std::vector<ColoredPixel>& pixels,
                     const MetalPatch& patch) {
  std::mt19937 random(patch.seed);
  const sf::IntRect area = patch.area;
  for (int row = 0; row < area.size.y; ++row) {
    if (!randomChance(random, BRUSH_STROKE_CHANCE)) {
      continue;
    }
    const int length =
        BRUSH_STROKE_SHORTEST +
        drawnBelow(
            random,
            std::max(1, static_cast<int>(static_cast<float>(area.size.x) *
                                         BRUSH_STROKE_LONGEST_SHARE)));
    const int start = drawnBelow(random, std::max(1, area.size.x - length));
    const sf::Color color = randomChance(random, BRUSH_LIGHT_CHANCE)
                                ? patch.shades.light
                                : patch.shades.dark;
    for (int column = start; column < std::min(area.size.x, start + length);
         ++column) {
      pixels.push_back({.position = area.position + sf::Vector2i(column, row),
                        .color = color});
    }
  }
}

void addWear(std::vector<ColoredPixel>& pixels, const MetalPatch& patch) {
  std::mt19937 random(patch.seed);
  addEdgeWear(pixels, patch, random);
  addScratches(pixels, patch, random);
  addGrime(pixels, patch, random);
}

void addRivet(std::vector<ColoredPixel>& pixels, sf::Vector2i topLeft) {
  pixels.push_back({.position = topLeft, .color = PALE_BLUE});
  pixels.push_back({.position = topLeft + sf::Vector2i(1, 0), .color = BLUE});
  pixels.push_back({.position = topLeft + sf::Vector2i(0, 1), .color = BLUE});
  pixels.push_back(
      {.position = topLeft + sf::Vector2i(1, 1), .color = HANGAR_SHADOW});
}

}  // namespace rtype::client
