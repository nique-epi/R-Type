#include "WidgetText.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <string_view>
#include "BarState.hpp"
#include "InterfaceConstants.hpp"
#include "TextMeasures.hpp"
#include "TextMetrics.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

int regularTextWidth(TextMetrics metrics, std::string_view text) {
  return regularWidthOf(metrics, text);
}

int centeredTextLeft(sf::IntRect box, std::string_view text,
                     TextMetrics metrics) {
  return box.position.x +
         ((box.size.x - regularTextWidth(metrics, text)) / SIDES_PER_AXIS);
}

int centeredTextTop(sf::IntRect box, TextMetrics metrics) {
  return box.position.y +
         ((box.size.y - lineHeightOf(metrics)) / SIDES_PER_AXIS);
}

int textBoxHeight(int nativeHeight, TextMetrics metrics) {
  return nativeHeight - TEXT_LINE_HEIGHT + lineHeightOf(metrics);
}

TextMetrics metricsOfControl(sf::IntRect area) {
  const int lineHeight = area.size.y - (CONTROL_HEIGHT - TEXT_LINE_HEIGHT);
  return TextMetrics{.halfPixelsPerFontPixel =
                         lineHeight * HALF_PIXELS_PER_PIXEL / TEXT_LINE_HEIGHT};
}

sf::Color barTextColor(BarState state) {
  switch (state) {
    case BarState::Selected:
      return SELECTED_BAR_TEXT_COLOR;
    case BarState::Pressed:
      return PRESSED_BAR_TEXT_COLOR;
    case BarState::Disabled:
      return DISABLED_BAR_TEXT_COLOR;
    case BarState::Idle:
      break;
  }
  return BAR_TEXT_COLOR;
}

}  // namespace rtype::client
