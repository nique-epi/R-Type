#pragma once

#include <cstddef>
#include <optional>
#include <vector>
#include "IInterfaceCanvas.hpp"
#include "ItemListLayout.hpp"
#include "ItemRow.hpp"

namespace rtype::client {

/**
 * @brief Rows under one another, each row a cell per column: what a screen
 * shows of items it reads elsewhere, such as the best scores of a server.
 *
 * It only paints. Rows past its capacity are left out, and a cell without a
 * column too. A pinned row stands under the rows, past a dashed line, for an
 * item that belongs with them without being one of them.
 */
class ItemList {
 public:
  explicit ItemList(ItemListLayout layout);

  void setRows(std::vector<ItemRow> rows);
  void setPinnedRow(std::optional<ItemRow> row);

  /** @returns The row of pixels where a row after the last one shown would
   * start: where a screen writes what it adds under the rows. */
  [[nodiscard]] int rowsEnd() const;

  void paint(IInterfaceCanvas& canvas) const;

  /** @brief Paints the dashed line under the rows, as paint() does above a
   * pinned row: for a screen that writes something else there. */
  void paintSeparator(IInterfaceCanvas& canvas) const;

 private:
  void paintRow(IInterfaceCanvas& canvas, const ItemRow& row, int top) const;

  ItemListLayout layout_;
  std::vector<ItemRow> rows_;
  std::optional<ItemRow> pinned_;
};

}  // namespace rtype::client
