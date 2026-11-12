#pragma once

#include <cstddef>
#include <vector>
#include "Bounds.hpp"
#include "CandidatePair.hpp"
#include "CellRange.hpp"
#include "GridAxis.hpp"

namespace rtype::engine {

/**
 * @brief Broad phase of the collisions: sorts boxes into the square cells of a
 * fixed area, and lists the pairs of boxes that share a cell.
 *
 * Only boxes that share a cell can overlap, so a caller tests these pairs with
 * overlaps() instead of every pair. The grid is filled again on every tick:
 * clear(), then insert() each box, then candidatePairs().
 *
 * A box is counted in every cell it covers. A pair sharing several cells is
 * listed once, by the cell that holds the top-left corner of the block of
 * cells the two boxes share (the rule 0fps gives for grids, "Collision
 * detection (part 2)"). A box that leaves the area, or whose center is not a
 * number, is counted in the cells along the edge it lies beyond, so boxes
 * beyond one edge are still paired with each other: the area sizes the grid,
 * it never drops a box. A box of negative width or height covers the cells
 * between its two edges.
 *
 * Filling costs one pass over the cells each box covers, listing one pass over
 * the pairs of boxes of each cell. Once the grid has held its largest fill, it
 * allocates nothing more.
 */
class UniformGrid {
 public:
  /**
   * @brief Lays square cells of side @p cellSide over @p area from its
   * top-left corner, as many as cover it, and at least one along each axis.
   *
   * @throws InvalidGridLayoutException when @p cellSide is not positive, or
   * when @p area needs more than MAXIMUM_GRID_LINES cells along an axis.
   */
  UniformGrid(const Bounds& area, float cellSide);

  /** @brief Forgets every box: the next box inserted is the box 0. */
  void clear();

  /** @brief Adds a box, numbered by the count of boxes added since clear(). */
  void insert(const Bounds& bounds);

  /**
   * @returns Every pair of boxes that share at least one cell, each listed
   * once, the earlier box first, in no order of pairs the caller may rely on.
   * The list stays valid until the next call of a member function.
   */
  [[nodiscard]] const std::vector<CandidatePair>& candidatePairs();

 private:
  [[nodiscard]] CellRange cellRangeOf(const Bounds& bounds) const;
  [[nodiscard]] std::size_t firstCellOf(const CellRange& range) const;
  [[nodiscard]] std::size_t ownerCellOf(const CandidatePair& pair) const;
  void sortBoxesIntoCells();
  void pairBoxesIn(std::size_t cell);

  /** @brief Calls @p visit with the index of each cell of @p range, row after
   * row, a cell being numbered row * column count + column. */
  template <typename Visit>
  void forEachCellOf(const CellRange& range, Visit visit) const {
    const std::size_t columnCount = range.lastColumn - range.firstColumn + 1;
    std::size_t rowStart = firstCellOf(range);
    for (std::size_t row = range.firstRow; row <= range.lastRow; ++row) {
      for (std::size_t cell = rowStart; cell < rowStart + columnCount; ++cell) {
        visit(cell);
      }
      rowStart += columns_.lineCount;
    }
  }

  GridAxis columns_;
  GridAxis rows_;
  /** @brief The cells each box covers, by the number of the box. */
  std::vector<CellRange> ranges_;
  /** @brief Where the boxes of each cell start in boxesByCell_, plus one last
   * entry where the last cell ends. While the cells are filled, the entry of
   * a cell first counts its boxes, then where it ends, and moves back by one
   * at each box placed, the last box first. */
  std::vector<std::size_t> cellStarts_;
  /** @brief The numbers of the boxes of every cell, cell after cell, each
   * cell in the order the boxes were inserted. */
  std::vector<std::size_t> boxesByCell_;
  std::vector<CandidatePair> pairs_;
};

}  // namespace rtype::engine
