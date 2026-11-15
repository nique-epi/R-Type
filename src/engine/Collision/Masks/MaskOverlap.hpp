#pragma once

#include "MaskArea.hpp"
#include "PlacedMask.hpp"

namespace rtype::engine {

/**
 * @returns true when a pixel drawn in @p first covers the same pixel of the
 * grid as a pixel drawn in @p second; false when the masks share no pixel of
 * the grid.
 *
 * Reads the rows the two masks share, MASK_WORD_BITS columns at a time, and
 * stops at the first word drawn in both: one `&` per shared row and word, at
 * most.
 */
[[nodiscard]] bool masksOverlap(const PlacedMask& first,
                                const PlacedMask& second);

/**
 * @returns true when a pixel drawn in @p placed lies in a cell of @p area;
 * false when the area covers none of its drawn pixels, or no cell at all.
 *
 * Reads the rows the mask and the area share as masksOverlap() does, and stops
 * at the first word holding a drawn pixel.
 */
[[nodiscard]] bool maskOverlapsArea(const PlacedMask& placed,
                                    const MaskArea& area);

}  // namespace rtype::engine
