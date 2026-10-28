#include "WidgetText.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Utf.hpp>
#include <string_view>
#include "BarState.hpp"
#include "InterfaceConstants.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

int regularTextWidth(std::string_view text) {
  return static_cast<int>(sf::Utf8::count(text.begin(), text.end())) *
         REGULAR_FONT_ADVANCE;
}

int centeredTextLeft(sf::IntRect box, std::string_view text) {
  return box.position.x +
         ((box.size.x - regularTextWidth(text)) / SIDES_PER_AXIS);
}

int centeredTextTop(sf::IntRect box) {
  return box.position.y + ((box.size.y - TEXT_LINE_HEIGHT) / SIDES_PER_AXIS);
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
