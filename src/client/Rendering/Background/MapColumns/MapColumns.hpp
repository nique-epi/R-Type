#pragma once

#include <span>
#include <vector>
#include "MapColumn.hpp"

namespace rtype::client {

/**
 * @returns The maps that show in the sky once a biome entered by
 * @p enteredPixels, from left to right, each with the column of its left
 * edge, on a whole pixel. The maps of @p widths are laid end to end and come
 * back to the first after the last; the first one starts at the right edge of
 * the sky when the biome begins, so a biome slides in from the right. None
 * for maps whose widths add up to nothing; a width is never negative.
 */
[[nodiscard]] std::vector<MapColumn> mapColumns(float enteredPixels,
                                                std::span<const int> widths);

}  // namespace rtype::client
