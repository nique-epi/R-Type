#include "Switch.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <utility>
#include "BarState.hpp"
#include "IInterfaceCanvas.hpp"
#include "RowControlConstants.hpp"
#include "SunkBox.hpp"
#include "TextStyle.hpp"
#include "ValueBinding.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

Switch::Switch(ValueBinding<bool> value) : value_(std::move(value)) {}

int Switch::width() const {
  return (SIDES_PER_AXIS * SWITCH_CELL_SIZE.x) +
         (SIDES_PER_AXIS * SWITCH_CELL_INSET);
}

void Switch::paint(IInterfaceCanvas& canvas, sf::IntRect area,
                   BarState row) const {
  SunkBox(area, SunkBox::Sides::Shaded).paint(canvas);
  const bool current = value_.read();
  for (const bool onSide : {false, true}) {
    const sf::IntRect cell = cellOf(onSide, area);
    const char* const word = onSide ? SWITCH_ON_TEXT : SWITCH_OFF_TEXT;
    const bool filled = onSide == current;
    if (filled) {
      canvas.fill(cell, row == BarState::Selected ? SELECTED_CURRENT_CELL_COLOR
                                                  : CURRENT_CELL_COLOR);
    }
    canvas.write(
        word,
        {centeredTextLeft(cell, word), cell.position.y + SWITCH_CELL_INSET},
        {.font = TextFont::Regular,
         .color = filled ? CURRENT_CELL_TEXT_COLOR : OTHER_CELL_TEXT_COLOR});
  }
}

bool Switch::keyPressed(const sf::Event::KeyPressed& key) {
  if (key.code != DECREASE_KEY && key.code != INCREASE_KEY) {
    return false;
  }
  value_.change(key.code == INCREASE_KEY);
  return true;
}

bool Switch::isActivatable() const { return true; }

void Switch::activate() { value_.write(!value_.read()); }

bool Switch::pointerPressed(sf::Vector2i pixel, sf::IntRect area) {
  const bool onOff = cellOf(false, area).contains(pixel);
  const bool onOn = cellOf(true, area).contains(pixel);
  if (!onOff && !onOn) {
    return false;
  }
  value_.change(onOn);
  return true;
}

sf::IntRect Switch::cellOf(bool onSide, sf::IntRect area) {
  const int left =
      area.position.x + SWITCH_CELL_INSET + (onSide ? SWITCH_CELL_SIZE.x : 0);
  return {{left, area.position.y + SWITCH_CELL_INSET}, SWITCH_CELL_SIZE};
}

}  // namespace rtype::client
