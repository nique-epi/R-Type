#pragma once

#include "ClothDirection.hpp"
#include "ClothPlace.hpp"

namespace rtype::client {

/**
 * @returns Where the cursor stands once moved from @p place towards
 * @p direction, one step: across the numbers, a number, then the line after
 * it, then the next number; down them, a row, the line under it, the next
 * row, then the street edge, the dozen under it and the even chance under
 * that. The zero takes the cursor from the first numbers, the columns from
 * the last ones, and gives it back on the row it came from. A step off the
 * cloth leaves the cursor where it stands.
 */
[[nodiscard]] ClothPlace placeAfter(ClothPlace place, ClothDirection direction);

}  // namespace rtype::client
