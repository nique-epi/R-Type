#pragma once

#include <span>
#include <vector>
#include "MapColumn.hpp"

namespace rtype::client {

/**
 * @returns The maps that show in the sky when the first one starts on the
 * column @p firstColumn, from left to right, each with the column of its
 * left edge. The maps of @p widths are laid end to end and come back to the
 * first after the last. None for maps whose widths add up to nothing; a width
 * is never negative.
 */
[[nodiscard]] std::vector<MapColumn> mapColumns(int firstColumn,
                                                std::span<const int> widths);

}  // namespace rtype::client
