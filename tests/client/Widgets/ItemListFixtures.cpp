#include "ItemListFixtures.hpp"
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "ItemCell.hpp"
#include "ItemList.hpp"
#include "ItemListTestConstants.hpp"
#include "ItemRow.hpp"
#include "RecordingInterfaceCanvas.hpp"

using rtype::client::ItemCell;
using rtype::client::ItemList;
using rtype::client::ItemRow;
using rtype::client::testing::CELL_COLOR;
using rtype::client::testing::LIST_LAYOUT;

ItemRow rowNamed(const std::string& name) {
  return ItemRow{.cells = {ItemCell{.text = "1", .color = CELL_COLOR},
                           ItemCell{.text = name, .color = CELL_COLOR}}};
}

InterfaceCanvasRecord paintedList(std::vector<ItemRow> rows,
                                  std::optional<ItemRow> pinned) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);
  ItemList list(LIST_LAYOUT);
  list.setRows(std::move(rows));
  list.setPinnedRow(std::move(pinned));
  list.paint(canvas);
  return record;
}
