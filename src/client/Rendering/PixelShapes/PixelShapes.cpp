#include "PixelShapes.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <vector>
#include "ColoredPixel.hpp"
#include "DitherConstants.hpp"
#include "DitherPatch.hpp"
#include "PixelDisc.hpp"
#include "PixelSegment.hpp"

namespace rtype::client {

namespace {

/** @returns @p value modulo BAYER_SIZE, 0 to BAYER_SIZE - 1 even for a
 * negative value. */
std::size_t bayerIndexOf(int value) {
  return static_cast<std::size_t>(((value % BAYER_SIZE) + BAYER_SIZE) %
                                  BAYER_SIZE);
}

/** @brief Adds a square segment.thickness pixels wide, in the color of
 * @p segment, whose middle is @p center. */
void addSquare(std::vector<ColoredPixel>& pixels, sf::Vector2i center,
               const PixelSegment& segment) {
  const int before = segment.thickness / 2;
  for (int row = 0; row < segment.thickness; ++row) {
    for (int column = 0; column < segment.thickness; ++column) {
      pixels.push_back(
          {.position = center + sf::Vector2i(column - before, row - before),
           .color = segment.color});
    }
  }
}

}  // namespace

void addSegment(std::vector<ColoredPixel>& pixels,
                const PixelSegment& segment) {
  sf::Vector2i point = segment.from;
  const int across = std::abs(segment.to.x - segment.from.x);
  const int down = -std::abs(segment.to.y - segment.from.y);
  const int stepX = segment.from.x < segment.to.x ? 1 : -1;
  const int stepY = segment.from.y < segment.to.y ? 1 : -1;
  int error = across + down;
  while (true) {
    addSquare(pixels, point, segment);
    if (point == segment.to) {
      return;
    }
    const int doubled = error + error;
    if (doubled >= down) {
      error += down;
      point.x += stepX;
    }
    if (doubled <= across) {
      error += across;
      point.y += stepY;
    }
  }
}

void addDisc(std::vector<ColoredPixel>& pixels, const PixelDisc& disc) {
  for (int row = -disc.radius; row <= disc.radius; ++row) {
    const int reach = static_cast<int>(std::floor(std::sqrt(static_cast<double>(
        (disc.radius * disc.radius) + disc.radius - (row * row)))));
    for (int column = -reach; column <= reach; ++column) {
      pixels.push_back({.position = disc.center + sf::Vector2i(column, row),
                        .color = disc.color});
    }
  }
}

void addDither(std::vector<ColoredPixel>& pixels, const DitherPatch& patch) {
  const int lastRow = patch.area.size.y - 1;
  for (int row = 0; row < patch.area.size.y; ++row) {
    const float down =
        lastRow > 0 ? static_cast<float>(row) / static_cast<float>(lastRow)
                    : 0.0F;
    const float share =
        patch.topShare + ((patch.bottomShare - patch.topShare) * down);
    for (int column = 0; column < patch.area.size.x; ++column) {
      const sf::Vector2i pixel =
          patch.area.position + sf::Vector2i(column, row);
      if (static_cast<double>(share) > bayerThresholdAt(pixel)) {
        pixels.push_back({.position = pixel, .color = patch.color});
      }
    }
  }
}

double bayerThresholdAt(sf::Vector2i pixel) {
  const int level =
      BAYER_MATRIX.at(bayerIndexOf(pixel.y)).at(bayerIndexOf(pixel.x));
  return (static_cast<double>(level) + BAYER_CELL_CENTRE) / BAYER_LEVELS;
}

sf::IntRect grownBy(sf::IntRect area, int margin) {
  return {area.position - sf::Vector2i(margin, margin),
          area.size + sf::Vector2i(margin + margin, margin + margin)};
}

}  // namespace rtype::client
