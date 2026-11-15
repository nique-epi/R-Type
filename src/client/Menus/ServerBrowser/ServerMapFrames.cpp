#include "ServerMapFrames.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cstddef>
#include <optional>
#include "GamePanelLayout.hpp"
#include "InterfaceConstants.hpp"
#include "PictureScroll.hpp"
#include "SectorMapConstants.hpp"
#include "ServerMapFrame.hpp"
#include "ServerMapScreenConstants.hpp"
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

ServerMapFrame serverMapFrameOf(TextMetrics metrics) {
  if (metrics == TextMetrics{}) {
    return {.subtitleTop = SERVER_MAP_SUBTITLE_ANCHOR.y,
            .subtitleWidth = std::nullopt,
            .mapShift = 0,
            .upperChipsTop = SERVER_MAP_CHIPS_TOP,
            .lowerChipsTop = SERVER_MAP_CHIPS_TOP,
            .height = INTERFACE_AREA.size.y};
  }
  const int lineHeight = lineHeightOf(metrics);
  const int subtitleTop =
      SERVER_MAP_TITLE_ANCHOR.y + lineHeight + SERVER_MAP_LINE_GAP;
  const int mapTop = std::max(
      SECTOR_MAP_AREA.position.y,
      subtitleTop +
          (SERVER_MAP_SUBTITLE_LINES * (lineHeight + SERVER_MAP_LINE_GAP)) +
          SERVER_MAP_SPACE_ABOVE_MAP);
  const int chipHeight = textBoxHeight(CHIP_HEIGHT, metrics);
  const int upperChipsTop = mapTop + SECTOR_MAP_AREA.size.y +
                            (SERVER_MAP_CHIPS_TOP - SECTOR_MAP_AREA.position.y -
                             SECTOR_MAP_AREA.size.y);
  const int lowerChipsTop = upperChipsTop + chipHeight + BUTTON_GAP;
  return {
      .subtitleTop = subtitleTop,
      .subtitleWidth = INTERFACE_AREA.size.x - (SIDES_PER_AXIS * SAFE_MARGIN),
      .mapShift = mapTop - SECTOR_MAP_AREA.position.y,
      .upperChipsTop = upperChipsTop,
      .lowerChipsTop = lowerChipsTop,
      .height = std::max(INTERFACE_AREA.size.y,
                         lowerChipsTop + chipHeight + SAFE_MARGIN)};
}

int pagedScrollOf(const sf::Event& event, PictureScroll scroll) {
  const auto* key = event.getIf<sf::Event::KeyPressed>();
  if (key == nullptr) {
    return scroll.scroll;
  }
  if (key->code == sf::Keyboard::Key::PageDown) {
    return std::min(scroll.last, scroll.scroll + SERVER_MAP_PAGE_STEP);
  }
  if (key->code == sf::Keyboard::Key::PageUp) {
    return std::max(0, scroll.scroll - SERVER_MAP_PAGE_STEP);
  }
  return scroll.scroll;
}

GamePanelLayout gamePanelLayoutOf(TextMetrics metrics) {
  if (metrics == TextMetrics{}) {
    return {.area = GAME_PANEL_AREA,
            .ruleTop = GAME_PANEL_RULE_TOP,
            .firstRow = GAME_PANEL_FIRST_ROW,
            .rowPitch = GAME_ROW_PITCH,
            .rowsShown = GAME_ROWS_SHOWN,
            .highlightHeight = GAME_ROW_HIGHLIGHT_HEIGHT,
            .stacked = false,
            .secondLine = 0,
            .descriptionTop = GAME_PANEL_DESCRIPTION_TOP,
            .descriptionWidth = std::nullopt};
  }
  const int lineHeight = lineHeightOf(metrics);
  const int ruleTop = GAME_PANEL_TITLE_TOP + lineHeight + SERVER_MAP_LINE_GAP;
  const int firstRow = ruleTop + GAME_PANEL_SPACE_UNDER_RULE;
  const int pitch = (SIDES_PER_AXIS * lineHeight) + GAME_PANEL_ROW_GAP;
  const int descriptionTop =
      SECTOR_MAP_AREA.size.y - GAME_PANEL_PADDING -
      (GAME_PANEL_DESCRIPTION_LINES * (lineHeight + SERVER_MAP_LINE_GAP));
  return {.area = SECTOR_MAP_AREA,
          .ruleTop = ruleTop,
          .firstRow = firstRow,
          .rowPitch = pitch,
          .rowsShown = static_cast<std::size_t>(
              std::max(1, (descriptionTop - firstRow) / pitch)),
          .highlightHeight = pitch - GAME_ROW_HIGHLIGHT_RISE,
          .stacked = true,
          .secondLine = lineHeight + SERVER_MAP_LINE_GAP,
          .descriptionTop = descriptionTop,
          .descriptionWidth =
              SECTOR_MAP_AREA.size.x - (SIDES_PER_AXIS * GAME_PANEL_PADDING)};
}

}  // namespace rtype::client
