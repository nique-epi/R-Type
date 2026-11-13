#include "RoomLayouts.hpp"
#include <algorithm>
#include <optional>
#include "InterfaceConstants.hpp"
#include "RoomLayout.hpp"
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

RoomLayout roomLayoutOf(int illustrationHeight, TextMetrics metrics) {
  const int barsTop =
      ROOM_ILLUSTRATION_TOP +
      (illustrationHeight > 0 ? illustrationHeight + SPACE_ABOVE_ROOM_BARS : 0);
  const int barHeight = textBoxHeight(BUTTON_SIZE.y, metrics);
  const int statusTop = barsTop + barHeight + BUTTON_GAP;
  const int chipHeight = textBoxHeight(CHIP_HEIGHT, metrics);
  if (metrics == TextMetrics{}) {
    const int chipsTop = INTERFACE_AREA.size.y - SAFE_MARGIN - chipHeight;
    return {.barsTop = barsTop,
            .barHeight = barHeight,
            .statusTop = statusTop,
            .statusWidth = std::nullopt,
            .chipHeight = chipHeight,
            .upperChipsTop = chipsTop,
            .lowerChipsTop = chipsTop,
            .height = INTERFACE_AREA.size.y};
  }
  const int upperChipsTop =
      statusTop +
      (ROOM_STATUS_LINES * (lineHeightOf(metrics) + DESCRIPTION_LINE_GAP)) +
      SPACE_ABOVE_ROOM_BARS;
  const int lowerChipsTop = upperChipsTop + chipHeight + BUTTON_GAP;
  return {.barsTop = barsTop,
          .barHeight = barHeight,
          .statusTop = statusTop,
          .statusWidth = INTERFACE_AREA.size.x - (SIDES_PER_AXIS * SAFE_MARGIN),
          .chipHeight = chipHeight,
          .upperChipsTop = upperChipsTop,
          .lowerChipsTop = lowerChipsTop,
          .height = std::max(INTERFACE_AREA.size.y,
                             lowerChipsTop + chipHeight + SAFE_MARGIN)};
}

int lastScrollOf(const RoomLayout& layout) {
  return std::max(0, layout.height - INTERFACE_AREA.size.y);
}

}  // namespace rtype::client
