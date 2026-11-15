#pragma once

#include <cstddef>
#include <optional>
#include <string>
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
 * item that belongs with them without being one of them; a note stands there,
 * centered in small print, when there is no such row.
 */
class ItemList {
 public:
  explicit ItemList(ItemListLayout layout);

  void setRows(std::vector<ItemRow> rows);
  void setPinnedRow(std::optional<ItemRow> row);

  /** @brief Sets the note shown without a pinned row; empty for none. */
  void setPinnedNote(std::string note);

  void paint(IInterfaceCanvas& canvas) const;

  /** @brief Moves the rows shown by @p step down, up for a negative step,
   * when they do not all fit; never past the first or the last. Above the
   * size the fonts are drawn at, each row takes two lines, so fewer show. */
  void scrollBy(int step);

 private:
  /** @returns The row of pixels where a row after the last one shown would
   * start. */
  [[nodiscard]] int rowsEnd() const;
  void paintSeparator(IInterfaceCanvas& canvas) const;
  void paintRow(IInterfaceCanvas& canvas, const ItemRow& row, int top) const;

  /** @brief Paints the rows on two lines each, from the first scrolled to,
   * as many as the height of the list holds with the pinned row under. */
  void paintStacked(IInterfaceCanvas& canvas) const;
  void paintStackedRow(IInterfaceCanvas& canvas, const ItemRow& row,
                       int top) const;

  ItemListLayout layout_;
  std::vector<ItemRow> rows_;
  std::optional<ItemRow> pinned_;
  std::string pinnedNote_;
  std::size_t firstRow_{};
  /** @brief How many rows the last painting showed, which bounds a scroll. */
  mutable std::size_t shownRows_{};
};

}  // namespace rtype::client
