#include "PixelMask.hpp"
#include <cstddef>
#include <cstdint>
#include "EngineException.hpp"
#include "MaskConstants.hpp"
#include "MaskPixel.hpp"
#include "MaskSpan.hpp"

namespace rtype::engine {

namespace {

/** @returns @p side, a side a mask may have.
 * @throws InvalidPixelMaskException when it is below 1 or above
 * MAXIMUM_MASK_SIDE. */
int checkedSide(int side) {
  if (side < 1 || side > MAXIMUM_MASK_SIDE) {
    throw InvalidPixelMaskException(MAXIMUM_MASK_SIDE);
  }
  return side;
}

/** @returns How many words hold a row of @p width pixels. */
std::size_t wordsPerRowOf(int width) {
  return static_cast<std::size_t>((width + MASK_WORD_BITS - 1) /
                                  MASK_WORD_BITS);
}

/** @returns How far column @p column lies from the first pixel of its word. */
unsigned int placeInWordOf(int column) {
  return static_cast<unsigned int>(column % MASK_WORD_BITS);
}

/** @returns The bit of its word that holds column @p column. */
std::uint64_t bitOfColumn(int column) {
  return FIRST_PIXEL_OF_WORD >> placeInWordOf(column);
}

}  // namespace

PixelMask::PixelMask(int width, int height)
    : width_(checkedSide(width)),
      height_(checkedSide(height)),
      wordsPerRow_(wordsPerRowOf(width_)),
      words_(wordsPerRow_ * static_cast<std::size_t>(height_), 0) {}

int PixelMask::width() const { return width_; }

int PixelMask::height() const { return height_; }

void PixelMask::markDrawn(MaskPixel pixel) {
  if (!isInside(pixel)) {
    throw InvalidPixelMaskException(MAXIMUM_MASK_SIDE);
  }
  words_[wordIndexOf(pixel)] |= bitOfColumn(pixel.x);
}

bool PixelMask::isDrawn(MaskPixel pixel) const {
  return isInside(pixel) &&
         (words_[wordIndexOf(pixel)] & bitOfColumn(pixel.x)) != 0;
}

std::uint64_t PixelMask::bitsOf(MaskSpan span) const {
  if (!isInside(span)) {
    throw InvalidPixelMaskException(MAXIMUM_MASK_SIDE);
  }
  const std::size_t index =
      wordIndexOf(MaskPixel{.x = span.first, .y = span.row});
  const unsigned int shift = placeInWordOf(span.first);
  const auto count = static_cast<unsigned int>(span.count);
  std::uint64_t window = words_[index] << shift;
  if (shift + count > MASK_WORD_SHIFT) {
    window |= words_[index + 1] >> (MASK_WORD_SHIFT - shift);
  }
  return window >> (MASK_WORD_SHIFT - count);
}

bool PixelMask::isInside(MaskPixel pixel) const {
  return pixel.x >= 0 && pixel.x < width_ && pixel.y >= 0 && pixel.y < height_;
}

bool PixelMask::isInside(MaskSpan span) const {
  return span.count >= 1 && span.count <= MASK_WORD_BITS && span.first >= 0 &&
         span.first + span.count <= width_ && span.row >= 0 &&
         span.row < height_;
}

std::size_t PixelMask::wordIndexOf(MaskPixel pixel) const {
  return (static_cast<std::size_t>(pixel.y) * wordsPerRow_) +
         static_cast<std::size_t>(pixel.x / MASK_WORD_BITS);
}

}  // namespace rtype::engine
