#include "UniformGrid.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>
#include "Bounds.hpp"
#include "CandidatePair.hpp"
#include "CellRange.hpp"
#include "CollisionConstants.hpp"
#include "EngineException.hpp"
#include "GridAxis.hpp"
#include "GridConstants.hpp"

namespace rtype::engine {

namespace {

float checkedCellSide(float cellSide) {
  if (!(cellSide > 0.0F)) {
    throw InvalidGridLayoutException(MAXIMUM_GRID_LINES);
  }
  return cellSide;
}

std::size_t lineCountOf(float lengthInCells) {
  if (!(lengthInCells <= static_cast<float>(MAXIMUM_GRID_LINES))) {
    throw InvalidGridLayoutException(MAXIMUM_GRID_LINES);
  }
  return static_cast<std::size_t>(std::max(std::ceil(lengthInCells), 1.0F));
}

GridAxis horizontalAxisOf(const Bounds& area, float cellSide) {
  const float side = checkedCellSide(cellSide);
  return GridAxis{.start = area.centerX - (area.width * HALF),
                  .cellSide = side,
                  .lineCount = lineCountOf(area.width / side)};
}

GridAxis verticalAxisOf(const Bounds& area, float cellSide) {
  const float side = checkedCellSide(cellSide);
  return GridAxis{.start = area.centerY - (area.height * HALF),
                  .cellSide = side,
                  .lineCount = lineCountOf(area.height / side)};
}

std::size_t lineOf(float position, const GridAxis& axis) {
  const float offsetInCells = (position - axis.start) / axis.cellSide;
  const std::size_t lastLine = axis.lineCount - 1;
  if (!(offsetInCells >= 0.0F)) {
    return 0;
  }
  if (offsetInCells >= static_cast<float>(lastLine)) {
    return lastLine;
  }
  return static_cast<std::size_t>(offsetInCells);
}

}  // namespace

UniformGrid::UniformGrid(const Bounds& area, float cellSide)
    : columns_(horizontalAxisOf(area, cellSide)),
      rows_(verticalAxisOf(area, cellSide)),
      cellStarts_((columns_.lineCount * rows_.lineCount) + 1) {}

void UniformGrid::clear() { ranges_.clear(); }

void UniformGrid::insert(const Bounds& bounds) {
  ranges_.push_back(cellRangeOf(bounds));
}

const std::vector<CandidatePair>& UniformGrid::candidatePairs() {
  sortBoxesIntoCells();
  pairs_.clear();
  for (std::size_t cell = 0; cell + 1 < cellStarts_.size(); ++cell) {
    pairBoxesIn(cell);
  }
  return pairs_;
}

CellRange UniformGrid::cellRangeOf(const Bounds& bounds) const {
  const float halfWidth = bounds.width * HALF;
  const float halfHeight = bounds.height * HALF;
  const std::size_t left = lineOf(bounds.centerX - halfWidth, columns_);
  const std::size_t right = lineOf(bounds.centerX + halfWidth, columns_);
  const std::size_t top = lineOf(bounds.centerY - halfHeight, rows_);
  const std::size_t bottom = lineOf(bounds.centerY + halfHeight, rows_);
  return CellRange{.firstColumn = std::min(left, right),
                   .firstRow = std::min(top, bottom),
                   .lastColumn = std::max(left, right),
                   .lastRow = std::max(top, bottom)};
}

std::size_t UniformGrid::firstCellOf(const CellRange& range) const {
  return (range.firstRow * columns_.lineCount) + range.firstColumn;
}

std::size_t UniformGrid::ownerCellOf(const CandidatePair& pair) const {
  const CellRange& first = ranges_[pair.first];
  const CellRange& second = ranges_[pair.second];
  const std::size_t column = std::max(first.firstColumn, second.firstColumn);
  const std::size_t row = std::max(first.firstRow, second.firstRow);
  return firstCellOf(CellRange{.firstColumn = column,
                               .firstRow = row,
                               .lastColumn = column,
                               .lastRow = row});
}

void UniformGrid::sortBoxesIntoCells() {
  std::ranges::fill(cellStarts_, 0);
  for (const CellRange& range : ranges_) {
    forEachCellOf(range, [this](std::size_t cell) { ++cellStarts_[cell]; });
  }
  for (std::size_t cell = 1; cell < cellStarts_.size(); ++cell) {
    cellStarts_[cell] += cellStarts_[cell - 1];
  }
  boxesByCell_.resize(cellStarts_.back());
  for (std::size_t box = ranges_.size(); box > 0; --box) {
    forEachCellOf(ranges_[box - 1], [this, box](std::size_t cell) {
      --cellStarts_[cell];
      boxesByCell_[cellStarts_[cell]] = box - 1;
    });
  }
}

void UniformGrid::pairBoxesIn(std::size_t cell) {
  const std::size_t end = cellStarts_[cell + 1];
  for (std::size_t earlier = cellStarts_[cell]; earlier < end; ++earlier) {
    for (std::size_t later = earlier + 1; later < end; ++later) {
      const CandidatePair pair{.first = boxesByCell_[earlier],
                               .second = boxesByCell_[later]};
      if (ownerCellOf(pair) == cell) {
        pairs_.push_back(pair);
      }
    }
  }
}

}  // namespace rtype::engine
