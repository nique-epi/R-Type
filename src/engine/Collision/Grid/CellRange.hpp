#pragma once

#include <cstddef>

namespace rtype::engine {

/**
 * @brief The block of cells of a UniformGrid a box covers: its first and last
 * column and its first and last row, all included, the first never past the
 * last. A single cell is a block whose first and last lines are the same.
 */
struct CellRange {
  std::size_t firstColumn{};
  std::size_t firstRow{};
  std::size_t lastColumn{};
  std::size_t lastRow{};
};

}  // namespace rtype::engine
