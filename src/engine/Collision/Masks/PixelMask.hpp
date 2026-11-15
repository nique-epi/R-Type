#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "MaskPixel.hpp"
#include "MaskSpan.hpp"

namespace rtype::engine {

/**
 * @brief One bit per pixel of a sprite: set where the pixel is drawn, clear
 * where it is transparent. Two sprites touch where a pixel is drawn in both.
 *
 * A row keeps its pixels in words of MASK_WORD_BITS pixels: pixel x lies in
 * word x / MASK_WORD_BITS of its row, its column x % MASK_WORD_BITS counted
 * from the highest bit, so a word reads left to right like the drawing. The
 * bits past the width are never set.
 */
class PixelMask {
 public:
  /**
   * @brief A mask of @p width by @p height pixels, every one transparent.
   * @throws InvalidPixelMaskException when a side is below 1 or above
   * MAXIMUM_MASK_SIDE.
   */
  PixelMask(int width, int height);

  [[nodiscard]] int width() const;
  [[nodiscard]] int height() const;

  /** @brief Marks @p pixel drawn.
   * @throws InvalidPixelMaskException when it lies outside the mask. */
  void markDrawn(MaskPixel pixel);

  /** @returns true when @p pixel lies inside the mask and is drawn. */
  [[nodiscard]] bool isDrawn(MaskPixel pixel) const;

  /**
   * @returns The pixels of @p span, one bit each, its last pixel in the lowest
   * bit: two spans of the same count read from two masks line up bit for bit,
   * so `&` says whether a pixel is drawn in both.
   * @throws InvalidPixelMaskException when @p span covers 0 or more than
   * MASK_WORD_BITS pixels, or a pixel outside the mask.
   */
  [[nodiscard]] std::uint64_t bitsOf(MaskSpan span) const;

 private:
  [[nodiscard]] bool isInside(MaskPixel pixel) const;
  [[nodiscard]] bool isInside(MaskSpan span) const;
  [[nodiscard]] std::size_t wordIndexOf(MaskPixel pixel) const;

  int width_{};
  int height_{};
  std::size_t wordsPerRow_{};
  std::vector<std::uint64_t> words_;
};

}  // namespace rtype::engine
