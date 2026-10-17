#include "ChoiceSelector.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include "BarState.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceConstants.hpp"
#include "RowControlConstants.hpp"
#include "SunkBox.hpp"
#include "TextStyle.hpp"
#include "ValueBinding.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

ChoiceSelector::ChoiceSelector(std::vector<std::string> choices,
                               ValueBinding<std::size_t> value)
    : choices_(std::move(choices)), value_(std::move(value)) {}

int ChoiceSelector::width() const {
  return (SIDES_PER_AXIS * (REGULAR_FONT_ADVANCE + CONTROL_GAP)) + boxWidth();
}

void ChoiceSelector::paint(IInterfaceCanvas& canvas, sf::IntRect area,
                           BarState row) const {
  const bool selected = row == BarState::Selected;
  const std::size_t current = value_.read();
  const sf::IntRect box = boxArea(area);
  SunkBox(box, SunkBox::Sides::Shaded).paint(canvas);
  const std::string& text = choices_[current];
  canvas.write(
      text, {centeredTextLeft(box, text), box.position.y + CONTROL_TEXT_TOP},
      {.font = TextFont::Regular,
       .color = selected ? SELECTED_BOXED_VALUE_COLOR : BOXED_VALUE_COLOR});
  if (!selected) {
    return;
  }
  const TextStyle arrows{.font = TextFont::Regular, .color = barTextColor(row)};
  if (current > 0) {
    canvas.write(DECREASE_ARROW,
                 {area.position.x, area.position.y + CONTROL_TEXT_TOP}, arrows);
  }
  if (current + 1 < choices_.size()) {
    canvas.write(INCREASE_ARROW,
                 {box.position.x + box.size.x + CONTROL_GAP,
                  area.position.y + CONTROL_TEXT_TOP},
                 arrows);
  }
}

bool ChoiceSelector::keyPressed(const sf::Event::KeyPressed& key) {
  const std::size_t current = value_.read();
  if (key.code == DECREASE_KEY) {
    if (current > 0) {
      value_.change(current - 1);
    }
    return true;
  }
  if (key.code == INCREASE_KEY) {
    if (current + 1 < choices_.size()) {
      value_.change(current + 1);
    }
    return true;
  }
  return false;
}

bool ChoiceSelector::isActivatable() const { return true; }

void ChoiceSelector::activate() {
  value_.change((value_.read() + 1) % choices_.size());
}

bool ChoiceSelector::pointerPressed(sf::Vector2i pixel, sf::IntRect area) {
  const sf::IntRect box = boxArea(area);
  if (box.contains(pixel)) {
    activate();
    return true;
  }
  if (pixel.x < box.position.x) {
    return keyPressed(sf::Event::KeyPressed{.code = DECREASE_KEY});
  }
  return keyPressed(sf::Event::KeyPressed{.code = INCREASE_KEY});
}

int ChoiceSelector::boxWidth() const {
  int widest = 0;
  for (const std::string& choice : choices_) {
    widest = std::max(widest, regularTextWidth(choice));
  }
  return widest + SELECTOR_BOX_PADDING;
}

sf::IntRect ChoiceSelector::boxArea(sf::IntRect area) const {
  return {
      {area.position.x + REGULAR_FONT_ADVANCE + CONTROL_GAP, area.position.y},
      {boxWidth(), CONTROL_HEIGHT}};
}

}  // namespace rtype::client
