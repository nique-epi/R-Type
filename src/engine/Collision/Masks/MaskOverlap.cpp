#include "MaskOverlap.hpp"
#include <algorithm>
#include <cstdint>
#include "MaskConstants.hpp"
#include "MaskSpan.hpp"
#include "PixelMask.hpp"
#include "PlacedMask.hpp"

namespace rtype::engine {

namespace {

/** @returns The bits of @p placed over @p span, a span of the grid. */
std::uint64_t bitsOnGrid(const PlacedMask& placed, MaskSpan span) {
  return placed.mask->bitsOf(MaskSpan{.row = span.row - placed.top,
                                      .first = span.first - placed.left,
                                      .count = span.count});
}

/** @returns true when a pixel of @p shared, a span of the grid both masks
 * cover, is drawn in both. */
bool spanOverlaps(const PlacedMask& first, const PlacedMask& second,
                  MaskSpan shared) {
  const int end = shared.first + shared.count;
  for (int column = shared.first; column < end; column += MASK_WORD_BITS) {
    const MaskSpan word{.row = shared.row,
                        .first = column,
                        .count = std::min(MASK_WORD_BITS, end - column)};
    if ((bitsOnGrid(first, word) & bitsOnGrid(second, word)) != 0) {
      return true;
    }
  }
  return false;
}

}  // namespace

bool masksOverlap(const PlacedMask& first, const PlacedMask& second) {
  const PixelMask& firstMask = *first.mask;
  const PixelMask& secondMask = *second.mask;
  const int left = std::max(first.left, second.left);
  const int right = std::min(first.left + firstMask.width(),
                             second.left + secondMask.width());
  const int top = std::max(first.top, second.top);
  const int bottom = std::min(first.top + firstMask.height(),
                              second.top + secondMask.height());
  if (left >= right) {
    return false;
  }
  for (int row = top; row < bottom; ++row) {
    if (spanOverlaps(
            first, second,
            MaskSpan{.row = row, .first = left, .count = right - left})) {
      return true;
    }
  }
  return false;
}

}  // namespace rtype::engine
