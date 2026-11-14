#include "PanelLayouts.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cstddef>
#include "InterfaceConstants.hpp"
#include "TabbedPanelLayout.hpp"
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

namespace {

/** @returns How tall the strip under the panel is: its bars, or the two
 * lines of the description beside them when those are taller. */
int barStripHeightOf(TextMetrics metrics) {
  const int bar = textBoxHeight(PANEL_BAR_SIZE.y, metrics);
  if (metrics == TextMetrics{}) {
    return bar;
  }
  return std::max(bar, (DESCRIPTION_LINES_BESIDE_BARS * lineHeightOf(metrics)) +
                           DESCRIPTION_LINE_GAP);
}

}  // namespace

TabbedPanelLayout tabbedPanelLayoutOf(TextMetrics metrics) {
  const int lineHeight = lineHeightOf(metrics);
  const sf::Vector2i tabsTopLeft(
      SAFE_MARGIN, SAFE_MARGIN + lineHeight + SPACE_BELOW_HEADING +
                       RULE_SIZE.y + SPACE_BELOW_RULE);
  const int tabHeight = textBoxHeight(TAB_HEIGHT, metrics);
  const int barsTop =
      PANEL_BARS_TOP + PANEL_BAR_SIZE.y - barStripHeightOf(metrics);
  const sf::IntRect panel(
      {SAFE_MARGIN, tabsTopLeft.y + tabHeight},
      {TAB_STRIP_WIDTH, barsTop - BUTTON_GAP - (tabsTopLeft.y + tabHeight)});
  const sf::Vector2i rowSize(ROW_SIZE.x, textBoxHeight(ROW_SIZE.y, metrics));
  const int inside = panel.size.y - (SIDES_PER_AXIS * ROWS_PANEL_PADDING);
  return TabbedPanelLayout{
      .tabsTopLeft = tabsTopLeft,
      .tabHeight = tabHeight,
      .panel = panel,
      .firstRowTopLeft =
          panel.position + sf::Vector2i(ROWS_PANEL_PADDING, ROWS_PANEL_PADDING),
      .rowSize = rowSize,
      .rowCapacity =
          static_cast<std::size_t>((inside + ROW_GAP) / (rowSize.y + ROW_GAP)),
      .barsTop = barsTop,
      .barSize = {PANEL_BAR_SIZE.x, textBoxHeight(PANEL_BAR_SIZE.y, metrics)}};
}

sf::IntRect rowsViewportOf(const TabbedPanelLayout& layout) {
  const int pitch = layout.rowSize.y + ROW_GAP;
  return {layout.firstRowTopLeft,
          {layout.rowSize.x,
           (static_cast<int>(layout.rowCapacity) * pitch) - ROW_GAP}};
}

}  // namespace rtype::client
