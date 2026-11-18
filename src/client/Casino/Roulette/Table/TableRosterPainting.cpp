#include "TableRosterPainting.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>
#include "BackgroundColorConstants.hpp"
#include "CasinoPainting.hpp"
#include "IInterfaceCanvas.hpp"
#include "RouletteScreenConstants.hpp"
#include "TableRosterRow.hpp"
#include "TextStyle.hpp"

namespace rtype::client {

namespace {

/** @brief Paints @p row, its top at @p top. */
void paintRow(IInterfaceCanvas& canvas, const TableRosterRow& row, int top) {
  const int left = ROSTER_TOP_LEFT.x;
  const sf::IntRect square({left + ROSTER_SQUARE_LEFT, top + 1},
                           {ROSTER_SQUARE_SIZE, ROSTER_SQUARE_SIZE});
  canvas.fill(square, row.colors.ship);
  paintEdges(canvas, square, row.colors.dark);
  canvas.write(row.name, {left + ROSTER_NAME_LEFT, top},
               TextStyle{.font = TextFont::Small, .color = STAR_WHITE});
  canvas.write(row.value, {left + ROSTER_WIDTH - ROSTER_VALUE_MARGIN, top},
               TextStyle{.font = TextFont::Small,
                         .color = row.valueColor,
                         .alignment = TextAlignment::Right});
}

}  // namespace

void paintTableRoster(IInterfaceCanvas& canvas,
                      const std::vector<TableRosterRow>& rows,
                      const std::string& roomName) {
  if (roomName.empty()) {
    return;
  }
  const int height =
      ROSTER_EMPTY_HEIGHT + (ROSTER_ROW_HEIGHT * static_cast<int>(rows.size()));
  paintGoldPanel(canvas, {ROSTER_TOP_LEFT, {ROSTER_WIDTH, height}});
  paintNameTag(canvas, ROSTER_TOP_LEFT + ROSTER_TAG_OFFSET, roomName);
  int top = ROSTER_TOP_LEFT.y + ROSTER_FIRST_ROW;
  for (const TableRosterRow& row : rows) {
    paintRow(canvas, row, top);
    top += ROSTER_ROW_HEIGHT;
  }
}

}  // namespace rtype::client
