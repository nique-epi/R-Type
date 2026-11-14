#include "ShapeContacts.hpp"
#include <cmath>
#include "Bounds.hpp"
#include "CollisionConstants.hpp"
#include "ContactConstants.hpp"
#include "ContactShape.hpp"
#include "MaskArea.hpp"
#include "MaskOverlap.hpp"
#include "MaskPixel.hpp"
#include "Overlaps.hpp"
#include "PixelMask.hpp"
#include "PlacedMask.hpp"
#include "Position.hpp"

namespace rtype::game {

namespace {

/** @returns The nearest whole cell to @p units, in cells of the grid, less
 * half of @p cells: the first cell of @p cells cells centered on @p units. */
int firstCellAround(float units, int cells) {
  return static_cast<int>(
      std::lround((units / SPRITE_PIXEL_UNITS) -
                  (static_cast<float>(cells) * engine::HALF)));
}

/** @returns The first cell the inside of a run starting at @p low units
 * covers. */
int firstCoveredCell(float low) {
  return static_cast<int>(std::floor(low / SPRITE_PIXEL_UNITS));
}

/** @returns The cell just past the last one the inside of a run ending at
 * @p high units covers. */
int cellPastTheCovered(float high) {
  return static_cast<int>(std::ceil(high / SPRITE_PIXEL_UNITS));
}

}  // namespace

engine::PlacedMask placedMaskOf(const Position& position,
                                const engine::PixelMask& mask) {
  return engine::PlacedMask{.mask = &mask,
                            .left = firstCellAround(position.x, mask.width()),
                            .top = firstCellAround(position.y, mask.height())};
}

engine::MaskArea areaOf(const engine::Bounds& bounds) {
  const float halfWidth = bounds.width * engine::HALF;
  const float halfHeight = bounds.height * engine::HALF;
  const int left = firstCoveredCell(bounds.centerX - halfWidth);
  const int top = firstCoveredCell(bounds.centerY - halfHeight);
  return engine::MaskArea{
      .left = left,
      .top = top,
      .width = cellPastTheCovered(bounds.centerX + halfWidth) - left,
      .height = cellPastTheCovered(bounds.centerY + halfHeight) - top};
}

bool shapesTouch(const ContactShape& first, const ContactShape& second) {
  if (!engine::overlaps(first.bounds, second.bounds)) {
    return false;
  }
  if (first.mask.has_value() && second.mask.has_value()) {
    return engine::masksOverlap(*first.mask, *second.mask);
  }
  if (first.mask.has_value()) {
    return engine::maskOverlapsArea(*first.mask, areaOf(second.bounds));
  }
  if (second.mask.has_value()) {
    return engine::maskOverlapsArea(*second.mask, areaOf(first.bounds));
  }
  return true;
}

engine::Bounds cellBoundsOf(const engine::PlacedMask& placed,
                            engine::MaskPixel pixel) {
  return engine::Bounds{
      .centerX = (static_cast<float>(placed.left + pixel.x) + engine::HALF) *
                 SPRITE_PIXEL_UNITS,
      .centerY = (static_cast<float>(placed.top + pixel.y) + engine::HALF) *
                 SPRITE_PIXEL_UNITS,
      .width = SPRITE_PIXEL_UNITS,
      .height = SPRITE_PIXEL_UNITS};
}

}  // namespace rtype::game
