#pragma once

#include <cstdint>
#include <limits>

namespace rtype::engine {

/** @brief The columns one word of a PixelMask holds, the bits of its word
 * type: a row is read and compared this many pixels at a time. */
constexpr int MASK_WORD_BITS = std::numeric_limits<std::uint64_t>::digits;

/** @brief MASK_WORD_BITS as an unsigned count, to shift a word by. */
constexpr auto MASK_WORD_SHIFT = static_cast<unsigned int>(MASK_WORD_BITS);

/** @brief A word whose highest bit alone is set: the first, leftmost pixel of
 * the word. */
constexpr std::uint64_t FIRST_PIXEL_OF_WORD = std::uint64_t{1}
                                              << (MASK_WORD_SHIFT - 1U);

/** @brief The widest and tallest mask, in pixels: it bounds what a data file
 * can make a mask allocate, 8 KiB at most. */
constexpr int MAXIMUM_MASK_SIDE = 256;

}  // namespace rtype::engine
