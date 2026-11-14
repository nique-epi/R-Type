#pragma once

#include <cstddef>

/** @returns How many of the first ROWS_READ rows of the column @p fromSeam
 * columns right of the seam stay hidden. */
[[nodiscard]] std::size_t hiddenRowsOf(int fromSeam);
