#include "ItemList.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "IInterfaceCanvas.hpp"
#include "InkTag.hpp"
#include "ItemCell.hpp"
#include "ItemListColumn.hpp"
#include "ItemListConstants.hpp"
#include "ItemListLayout.hpp"
#include "ItemRow.hpp"
#include "Outline.hpp"
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
#include "TextStyle.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/** @returns A band across the rows of a list, past their sides. */
sf::IntRect bandOf(const ItemListLayout& layout, int top, int height) {
  return {{layout.topLeft.x - ITEM_ROW_MARGIN, top},
          {layout.width + (SIDES_PER_AXIS * ITEM_ROW_MARGIN), height}};
}

/** @returns From the top of a row of two lines to the top of the next one:
 * the two lines, the gap between them, and the room under the row. */
int stackedPitchOf(TextMetrics metrics) {
  return (SIDES_PER_AXIS * lineHeightOf(metrics)) + ITEM_STACKED_LINE_GAP +
         ITEM_STACKED_ROW_GAP;
}

/** @brief Writes the tag of a cell, when it has one, on ink right after its
 * text. */
void paintTag(IInterfaceCanvas& canvas, const ItemCell& cell,
              sf::Vector2i anchor) {
  if (!cell.tag.has_value()) {
    return;
  }
  const int left =
      anchor.x + canvas.textWidth(cell.text, TextFont::Regular) + ITEM_TAG_GAP;
  paintInkTag(canvas, InkTag{.word = *cell.tag,
                             .topLeft = {left, anchor.y - ITEM_TAG_RISE},
                             .wordColor = cell.color});
}

}  // namespace

ItemList::ItemList(ItemListLayout layout) : layout_(layout) {}

void ItemList::setRows(std::vector<ItemRow> rows) { rows_ = std::move(rows); }

void ItemList::setPinnedRow(std::optional<ItemRow> row) {
  pinned_ = std::move(row);
}

void ItemList::setPinnedNote(std::string note) {
  pinnedNote_ = std::move(note);
}

void ItemList::scrollBy(int step) {
  const std::size_t last =
      rows_.size() > shownRows_ ? rows_.size() - shownRows_ : 0;
  const auto moved = static_cast<std::ptrdiff_t>(firstRow_) + step;
  firstRow_ = static_cast<std::size_t>(
      std::clamp<std::ptrdiff_t>(moved, 0, static_cast<std::ptrdiff_t>(last)));
}

int ItemList::rowsEnd() const {
  return layout_.topLeft.y + (static_cast<int>(shownRows_) * layout_.rowPitch);
}

void ItemList::paint(IInterfaceCanvas& canvas) const {
  if (canvas.textMetrics() != TextMetrics{}) {
    paintStacked(canvas);
    return;
  }
  const std::size_t shown = std::min(rows_.size(), layout_.rowCapacity);
  shownRows_ = shown;
  for (std::size_t row = 0; row < shown; ++row) {
    paintRow(canvas, rows_[row],
             layout_.topLeft.y + (static_cast<int>(row) * layout_.rowPitch));
  }
  if (pinned_.has_value()) {
    paintSeparator(canvas);
    paintRow(canvas, *pinned_, rowsEnd() + ITEM_PINNED_ROW_DROP);
  } else if (!pinnedNote_.empty()) {
    paintSeparator(canvas);
    canvas.write(pinnedNote_,
                 {layout_.topLeft.x + (layout_.width / SIDES_PER_AXIS),
                  rowsEnd() + ITEM_PINNED_ROW_DROP},
                 {.font = TextFont::Small,
                  .color = ITEM_NOTE_COLOR,
                  .alignment = TextAlignment::Center});
  }
}

void ItemList::paintStacked(IInterfaceCanvas& canvas) const {
  const int pitch = stackedPitchOf(canvas.textMetrics());
  const int room = layout_.height > 0 ? layout_.height
                                      : static_cast<int>(layout_.rowCapacity) *
                                            layout_.rowPitch;
  const bool pinned = pinned_.has_value() || !pinnedNote_.empty();
  const auto fit =
      static_cast<std::size_t>(std::max(1, (room / pitch) - (pinned ? 1 : 0)));
  shownRows_ = std::min(rows_.size() - std::min(firstRow_, rows_.size()), fit);
  int top = layout_.topLeft.y;
  for (std::size_t row = 0; row < shownRows_; ++row) {
    paintStackedRow(canvas, rows_[firstRow_ + row], top);
    top += pitch;
  }
  if (pinned_.has_value()) {
    paintStackedRow(canvas, *pinned_, top + ITEM_PINNED_ROW_DROP);
  } else if (!pinnedNote_.empty()) {
    canvas.write(pinnedNote_,
                 {layout_.topLeft.x + (layout_.width / SIDES_PER_AXIS),
                  top + ITEM_PINNED_ROW_DROP},
                 {.font = TextFont::Small,
                  .color = ITEM_NOTE_COLOR,
                  .alignment = TextAlignment::Center});
  }
}

void ItemList::paintStackedRow(IInterfaceCanvas& canvas, const ItemRow& row,
                               int top) const {
  const int lineHeight = lineHeightOf(canvas.textMetrics());
  if (row.highlight.has_value()) {
    canvas.fill(
        bandOf(layout_, top - ITEM_ROW_SPACE_ABOVE,
               stackedPitchOf(canvas.textMetrics()) - ITEM_ROW_SPACE_ABOVE),
        *row.highlight);
  }
  const std::size_t cells = std::min(row.cells.size(), layout_.columns.size());
  for (std::size_t cell = 0; cell < cells; ++cell) {
    const ItemListColumn& column = layout_.columns[cell];
    const sf::Vector2i anchor(
        layout_.topLeft.x + (column.stackedLetters * lineHeight),
        top + (column.stackedLine * (lineHeight + ITEM_STACKED_LINE_GAP)));
    canvas.write(row.cells[cell].text, anchor,
                 {.font = column.font,
                  .color = row.cells[cell].color,
                  .alignment = column.alignment});
    paintTag(canvas, row.cells[cell], anchor);
  }
}

void ItemList::paintSeparator(IInterfaceCanvas& canvas) const {
  Outline(bandOf(layout_, rowsEnd() - ITEM_PIN_LINE_RISE, 1),
          Outline::Line::Dashed)
      .paint(canvas, ITEM_PIN_LINE_COLOR);
}

void ItemList::paintRow(IInterfaceCanvas& canvas, const ItemRow& row,
                        int top) const {
  if (row.highlight.has_value()) {
    canvas.fill(bandOf(layout_, top - ITEM_ROW_SPACE_ABOVE, ITEM_ROW_HEIGHT),
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
    paintTag(canvas, row.cells[cell], anchor);
  }
}

}  // namespace rtype::client
