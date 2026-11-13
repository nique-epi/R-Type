#pragma once

#include <cstddef>

namespace rtype::client {

/** @brief The rows of a grid a screen shows at a time: the first one shown,
 * how many show (at least one), and how many rows the grid holds. */
struct RowWindow {
  std::size_t first{};
  std::size_t shown{};
  std::size_t count{};

  bool operator==(const RowWindow& other) const = default;
};

/** @returns The last row @p rows may start from: 0 when every row shows. */
[[nodiscard]] std::size_t lastFirstRowOf(RowWindow rows);

/** @returns Whether @p rows shows @p row. */
[[nodiscard]] bool showsRow(RowWindow rows, std::size_t row);

/** @returns @p rows moved the least so that @p row shows: unchanged when it
 * already does. */
[[nodiscard]] RowWindow rowsShowing(RowWindow rows, std::size_t row);

/** @returns @p rows moved @p step rows down, up for a negative step, never
 * past its first or its last start. */
[[nodiscard]] RowWindow rowsScrolledBy(RowWindow rows, int step);

}  // namespace rtype::client
