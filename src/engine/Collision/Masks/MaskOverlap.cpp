#include "MaskOverlap.hpp"
#include <algorithm>
#include <cstdint>
#include "MaskArea.hpp"
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

/** @returns true when @p test holds for one piece of @p span, a span of the
 * grid read MASK_WORD_BITS columns at a time from its first column. */
template <typename WordTest>
bool anyWordOf(MaskSpan span, const WordTest& test) {
  const int end = span.first + span.count;
  for (int column = span.first; column < end; column += MASK_WORD_BITS) {
    const MaskSpan word{.row = span.row,
                        .first = column,
                        .count = std::min(MASK_WORD_BITS, end - column)};
    if (test(word)) {
      return true;
    }
  }
  return false;
}

/** @returns true when a pixel of @p shared, a span of the grid both masks
 * cover, is drawn in both. */
bool spanOverlaps(const PlacedMask& first, const PlacedMask& second,
                  MaskSpan shared) {
  return anyWordOf(shared, [&first, &second](MaskSpan word) {
    return (bitsOnGrid(first, word) & bitsOnGrid(second, word)) != 0;
  });
}

/** @returns true when a pixel of @p shared, a span of the grid @p placed
 * covers, is drawn in it. */
bool spanHoldsADrawnPixel(const PlacedMask& placed, MaskSpan shared) {
  return anyWordOf(shared, [&placed](MaskSpan word) {
    return bitsOnGrid(placed, word) != 0;
  });
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

bool maskOverlapsArea(const PlacedMask& placed, const MaskArea& area) {
  const PixelMask& mask = *placed.mask;
  const int left = std::max(placed.left, area.left);
  const int right =
      std::min(placed.left + mask.width(), area.left + area.width);
  const int top = std::max(placed.top, area.top);
  const int bottom =
      std::min(placed.top + mask.height(), area.top + area.height);
  if (left >= right) {
    return false;
  }
  for (int row = top; row < bottom; ++row) {
    if (spanHoldsADrawnPixel(
            placed,
            MaskSpan{.row = row, .first = left, .count = right - left})) {
      return true;
    }
  }
  return false;
}

}  // namespace rtype::engine
