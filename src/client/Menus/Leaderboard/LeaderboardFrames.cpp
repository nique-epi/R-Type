#include "LeaderboardFrames.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include "InterfaceConstants.hpp"
#include "ItemListLayout.hpp"
#include "LeaderboardFrame.hpp"
#include "LeaderboardScreenConstants.hpp"
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

namespace {

/** @returns The leaderboard as it was drawn. */
LeaderboardFrame drawnFrame() {
  return LeaderboardFrame{.metrics = TextMetrics{},
                          .panel = LEADERBOARD_PANEL,
                          .tabsTopLeft = LEADERBOARD_TABS_TOP_LEFT,
                          .tabsWidth = LEADERBOARD_TABS_WIDTH,
                          .list = LEADERBOARD_LIST,
                          .chipsTop = LEADERBOARD_CHIPS_TOP,
                          .chipsRight = LEADERBOARD_CHIPS_RIGHT,
                          .summaryAnchor = FILTER_SUMMARY_ANCHOR,
                          .joinBar = JOIN_BAR,
                          .messageLine = MESSAGE_LINE_ANCHOR,
                          .messageDetail = MESSAGE_DETAIL_ANCHOR,
                          .asking = ASKING_ANCHOR};
}

/** @brief Places the lines in place of rows and the bar under them in the
 * panel of @p frame: the line over the middle of the panel, the small print
 * under it, the bar under both. */
void placeMessages(LeaderboardFrame& frame) {
  const int lineHeight = lineHeightOf(frame.metrics);
  const int middle =
      frame.panel.position.x + (frame.panel.size.x / SIDES_PER_AXIS);
  const int lineTop = frame.panel.position.y +
                      (frame.panel.size.y / SIDES_PER_AXIS) - lineHeight;
  frame.messageLine = {middle, lineTop};
  frame.messageDetail = {middle, lineTop + lineHeight + LEADERBOARD_LARGE_GAP};
  frame.asking = {middle - (ASKING_TEXT_LETTERS * lineHeight / SIDES_PER_AXIS),
                  lineTop + LEADERBOARD_LARGE_GAP};
  const sf::Vector2i bar(
      std::max(BUTTON_SIZE.x, regularWidthOf(frame.metrics, JOIN_LABEL) +
                                  (SIDES_PER_AXIS * BUTTON_TEXT_OFFSET.x)),
      textBoxHeight(BUTTON_SIZE.y, frame.metrics));
  frame.joinBar = {{middle - (bar.x / SIDES_PER_AXIS),
                    frame.messageDetail.y + lineHeight + BUTTON_GAP},
                   bar};
}

}  // namespace

LeaderboardFrame leaderboardFrameOf(TextMetrics metrics) {
  if (metrics == TextMetrics{}) {
    return drawnFrame();
  }
  const int lineHeight = lineHeightOf(metrics);
  LeaderboardFrame frame = drawnFrame();
  frame.metrics = metrics;
  frame.tabsTopLeft = {SAFE_MARGIN, LEADERBOARD_TABS_TOP_LEFT.y};
  frame.tabsWidth = INTERFACE_AREA.size.x - (SIDES_PER_AXIS * SAFE_MARGIN);
  frame.chipsTop =
      LEADERBOARD_CHIPS_BOTTOM - textBoxHeight(CHIP_HEIGHT, metrics);
  frame.chipsRight = SAFE_MARGIN + frame.tabsWidth;
  frame.summaryAnchor = {SAFE_MARGIN,
                         frame.chipsTop - lineHeight - LEADERBOARD_LARGE_GAP};
  const int panelTop = frame.tabsTopLeft.y + textBoxHeight(TAB_HEIGHT, metrics);
  const int panelBottom = frame.summaryAnchor.y - LEADERBOARD_LARGE_GAP;
  frame.panel = {{SAFE_MARGIN, panelTop},
                 {frame.tabsWidth, panelBottom - panelTop}};
  frame.list.topLeft = {SAFE_MARGIN + LEADERBOARD_LARGE_LIST_INSET,
                        panelTop + LEADERBOARD_LARGE_LIST_TOP};
  frame.list.width =
      frame.tabsWidth - (SIDES_PER_AXIS * LEADERBOARD_LARGE_LIST_INSET);
  frame.list.height = panelBottom - frame.list.topLeft.y;
  placeMessages(frame);
  return frame;
}

}  // namespace rtype::client
