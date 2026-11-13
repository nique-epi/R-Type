#include "RowWindow.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace rtype::client {

std::size_t lastFirstRowOf(RowWindow rows) {
  return rows.count > rows.shown ? rows.count - rows.shown : 0;
}

bool showsRow(RowWindow rows, std::size_t row) {
  return row >= rows.first && row < rows.first + rows.shown;
}

RowWindow rowsShowing(RowWindow rows, std::size_t row) {
  if (row < rows.first) {
    rows.first = row;
  } else if (row >= rows.first + rows.shown) {
    rows.first = row + 1 - rows.shown;
  }
  rows.first = std::min(rows.first, lastFirstRowOf(rows));
  return rows;
}

RowWindow rowsScrolledBy(RowWindow rows, int step) {
  const auto last = static_cast<std::int64_t>(lastFirstRowOf(rows));
  rows.first = static_cast<std::size_t>(std::clamp(
      static_cast<std::int64_t>(rows.first) + step, std::int64_t{0}, last));
  return rows;
}

}  // namespace rtype::client
