#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include "ColoredPixel.hpp"
#include "DitherPatch.hpp"
#include "PixelDisc.hpp"
#include "PixelSegment.hpp"

namespace rtype::client {

/** @brief Adds the pixels of @p segment: Bresenham's line from its first
 * pixel to its last, both included, a square thickness wide on each step. */
void addSegment(std::vector<ColoredPixel>& pixels, const PixelSegment& segment);

/** @brief Adds the pixels of @p disc, row by row. */
void addDisc(std::vector<ColoredPixel>& pixels, const PixelDisc& disc);

/** @brief Adds the pixels of @p patch whose share is above the threshold of
 * BAYER_MATRIX at their place. */
void addDither(std::vector<ColoredPixel>& pixels, const DitherPatch& patch);

/** @returns The threshold of the dither at @p pixel, BAYER_CELL_CENTRE /
 * BAYER_LEVELS to (BAYER_LEVELS - BAYER_CELL_CENTRE) / BAYER_LEVELS. */
[[nodiscard]] double bayerThresholdAt(sf::Vector2i pixel);

/** @returns @p area grown by @p margin on every side; shrunk when
 * negative. */
[[nodiscard]] sf::IntRect grownBy(sf::IntRect area, int margin);

}  // namespace rtype::client
