#pragma once

#include <SFML/System/Vector2.hpp>
#include <vector>
#include "ColoredPixel.hpp"
#include "MetalPatch.hpp"

namespace rtype::client {

/** @brief Adds short horizontal strokes, light or dark, across @p patch: on
 * about BRUSH_STROKE_CHANCE of its rows, one stroke each. */
void addBrushStrokes(std::vector<ColoredPixel>& pixels,
                     const MetalPatch& patch);

/** @brief Adds wear to @p patch: light pixels along its sides, a few
 * scratches across it, and grime that thickens towards its bottom. */
void addWear(std::vector<ColoredPixel>& pixels, const MetalPatch& patch);

/** @brief Adds a rivet two pixels wide whose top-left pixel is @p topLeft. */
void addRivet(std::vector<ColoredPixel>& pixels, sf::Vector2i topLeft);

}  // namespace rtype::client
