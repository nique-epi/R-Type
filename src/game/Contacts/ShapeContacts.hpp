#pragma once

#include "Bounds.hpp"
#include "ContactShape.hpp"
#include "MaskArea.hpp"
#include "PixelMask.hpp"
#include "PlacedMask.hpp"
#include "Position.hpp"

namespace rtype::game {

/**
 * @returns @p mask laid centered on @p position, on the grid of the sprite
 * pixels (SPRITE_PIXEL_UNITS units a cell, cell 0 starting at 0).
 *
 * Its top-left cell is the nearest whole cell to the position less half the
 * mask, halves rounded away from zero: the cell the client draws the top-left
 * pixel of the sprite on (nearestPixel()), so the mask lies under the pixels
 * the player sees.
 */
[[nodiscard]] engine::PlacedMask placedMaskOf(const Position& position,
                                              const engine::PixelMask& mask);

/** @returns The cells of the grid of the sprite pixels whose inside @p bounds
 * covers: a cell its edge only touches is left out, as engine::overlaps()
 * leaves out boxes that only touch. */
[[nodiscard]] engine::MaskArea areaOf(const engine::Bounds& bounds);

/**
 * @returns true when two shapes touch: their boxes overlap, then, when both
 * have a mask, a drawn pixel of each lies on the same cell; when one alone has
 * a mask, one of its drawn pixels lies in a cell the other box covers. Two
 * shapes without a mask touch when their boxes overlap.
 */
[[nodiscard]] bool shapesTouch(const ContactShape& first,
                               const ContactShape& second);

}  // namespace rtype::game
