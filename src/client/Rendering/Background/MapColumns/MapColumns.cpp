#include "MapColumns.hpp"
#include <cmath>
#include <cstddef>
#include <span>
#include <vector>
#include "BackgroundConstants.hpp"
#include "MapColumn.hpp"

namespace rtype::client {

namespace {

int totalWidthOf(std::span<const int> widths) {
  int total = 0;
  for (const int width : widths) {
    total += width;
  }
  return total;
}

MapColumn firstShown(int passedLeftEdge, std::span<const int> widths,
                     int totalWidth) {
  if (passedLeftEdge < 0) {
    return MapColumn{.map = 0, .column = -passedLeftEdge};
  }
  int intoMap = passedLeftEdge % totalWidth;
  std::size_t map = 0;
  while (intoMap >= widths[map]) {
    intoMap -= widths[map];
    ++map;
  }
  return MapColumn{.map = map, .column = -intoMap};
}

}  // namespace

std::vector<MapColumn> mapColumns(float enteredPixels,
                                  std::span<const int> widths) {
  std::vector<MapColumn> columns;
  const int totalWidth = totalWidthOf(widths);
  if (totalWidth <= 0) {
    return columns;
  }
  const int skyWidth = static_cast<int>(BACKGROUND_WIDTH);
  MapColumn next =
      firstShown(static_cast<int>(std::floor(enteredPixels)) - skyWidth, widths,
                 totalWidth);
  while (next.column < skyWidth) {
    columns.push_back(next);
    next.column += widths[next.map];
    next.map = (next.map + 1) % widths.size();
  }
  return columns;
}

}  // namespace rtype::client
