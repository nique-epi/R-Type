#include "ItemList.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>
#include "IInterfaceCanvas.hpp"
#include "ItemCell.hpp"
#include "ItemListColumn.hpp"
#include "ItemListConstants.hpp"
#include "ItemListLayout.hpp"
#include "ItemRow.hpp"
#include "Outline.hpp"
#include "TextStyle.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @brief Writes the tag of a cell on ink, right after its text. */
void paintTag(IInterfaceCanvas& canvas, const ItemCell& cell,
              sf::Vector2i anchor) {
  const int left =
      anchor.x + canvas.textWidth(cell.text, TextFont::Regular) + ITEM_TAG_GAP;
  const sf::Vector2i topLeft(left, anchor.y - ITEM_TAG_RISE);
  canvas.fill(
      sf::IntRect(topLeft, {canvas.textWidth(cell.tag, TextFont::Small) +
                                ITEM_TAG_PADDING,
                            ITEM_TAG_HEIGHT}),
      ITEM_TAG_COLOR);
  canvas.write(cell.tag, topLeft + sf::Vector2i(ITEM_TAG_TEXT_LEFT, 0),
               {.font = TextFont::Small, .color = cell.color});
}

}  // namespace

ItemList::ItemList(ItemListLayout layout) : layout_(layout) {}

void ItemList::setRows(std::vector<ItemRow> rows) { rows_ = std::move(rows); }

void ItemList::setPinnedRow(std::optional<ItemRow> row) {
  pinned_ = std::move(row);
}

int ItemList::rowsEnd() const {
  const std::size_t shown = std::min(rows_.size(), layout_.rowCapacity);
  return layout_.topLeft.y + (static_cast<int>(shown) * layout_.rowPitch);
}

void ItemList::paint(IInterfaceCanvas& canvas) const {
  const std::size_t shown = std::min(rows_.size(), layout_.rowCapacity);
  for (std::size_t row = 0; row < shown; ++row) {
    paintRow(canvas, rows_[row],
             layout_.topLeft.y + (static_cast<int>(row) * layout_.rowPitch));
  }
  if (pinned_.has_value()) {
    paintSeparator(canvas);
    paintRow(canvas, *pinned_, rowsEnd() + ITEM_PINNED_ROW_DROP);
  }
}

void ItemList::paintSeparator(IInterfaceCanvas& canvas) const {
  const sf::IntRect line(
      {layout_.topLeft.x - ITEM_ROW_MARGIN, rowsEnd() - ITEM_PIN_LINE_RISE},
      {layout_.width + (SIDES_PER_AXIS * ITEM_ROW_MARGIN), 1});
  Outline(line, Outline::Line::Dashed).paint(canvas, ITEM_PIN_LINE_COLOR);
}

void ItemList::paintRow(IInterfaceCanvas& canvas, const ItemRow& row,
                        int top) const {
  if (row.highlight.has_value()) {
    canvas.fill(sf::IntRect({layout_.topLeft.x - ITEM_ROW_MARGIN,
                             top - ITEM_ROW_SPACE_ABOVE},
                            {layout_.width + (SIDES_PER_AXIS * ITEM_ROW_MARGIN),
                             ITEM_ROW_HEIGHT}),
                *row.highlight);
  }
  const std::size_t cells = std::min(row.cells.size(), layout_.columns.size());
  for (std::size_t cell = 0; cell < cells; ++cell) {
    const ItemListColumn& column = layout_.columns[cell];
    const sf::Vector2i anchor(layout_.topLeft.x + column.anchor, top);
    canvas.write(row.cells[cell].text, anchor,
                 {.font = column.font,
                  .color = row.cells[cell].color,
                  .alignment = column.alignment});
    if (!row.cells[cell].tag.empty()) {
      paintTag(canvas, row.cells[cell], anchor);
    }
  }
}

}  // namespace rtype::client
