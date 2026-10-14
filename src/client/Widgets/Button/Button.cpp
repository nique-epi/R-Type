#include "Button.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <string>
#include <utility>
#include "ButtonState.hpp"
#include "ButtonText.hpp"
#include "ChamferedBox.hpp"
#include "CheckMark.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceConstants.hpp"
#include "TextStyle.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

Button::Button(ButtonText text, sf::IntRect area, std::function<void()> action)
    : text_(std::move(text)), area_(area), action_(std::move(action)) {}

bool Button::isEnabled() const { return enabled_; }

void Button::setEnabled(bool enabled) { enabled_ = enabled; }

void Button::setMark(CheckMark mark) { mark_ = mark; }

bool Button::contains(sf::Vector2i pixel) const {
  return area_.contains(pixel);
}

const std::string& Button::description() const { return text_.description; }

void Button::activate() const {
  if (action_ != nullptr) {
    action_();
  }
}

void Button::paint(IInterfaceCanvas& canvas, ButtonState state) const {
  const Look look = lookOf(state);
  paintChamferedBox(canvas, area_, look.bar);
  sf::Vector2i labelAnchor =
      area_.position + BUTTON_TEXT_OFFSET + sf::Vector2i(0, look.textDrop);
  if (mark_ != CheckMark::None) {
    paintCheckBox(canvas, labelAnchor, look.text);
    labelAnchor.x += CHECK_BOX_SIZE + CHECK_BOX_GAP;
  }
  canvas.write(text_.label, labelAnchor,
               {.font = TextFont::Regular, .color = look.text});
}

Button::Look Button::lookOf(ButtonState state) {
  switch (state) {
    case ButtonState::Selected:
      return {.bar = {.border = SELECTED_BAR_BORDER_COLOR,
                      .inside = SELECTED_BAR_INSIDE_COLOR},
              .text = SELECTED_BAR_TEXT_COLOR,
              .textDrop = 0};
    case ButtonState::Pressed:
      return {.bar = {.border = SELECTED_BAR_BORDER_COLOR,
                      .inside = PRESSED_BAR_INSIDE_COLOR},
              .text = PRESSED_BAR_TEXT_COLOR,
              .textDrop = PRESSED_TEXT_DROP};
    case ButtonState::Disabled:
      return {.bar = {.border = DISABLED_BAR_BORDER_COLOR,
                      .inside = BAR_INSIDE_COLOR},
              .text = DISABLED_BAR_TEXT_COLOR,
              .textDrop = 0};
    case ButtonState::Idle:
      break;
  }
  return {.bar = {.border = BAR_BORDER_COLOR, .inside = BAR_INSIDE_COLOR},
          .text = BAR_TEXT_COLOR,
          .textDrop = 0};
}

void Button::paintCheckBox(IInterfaceCanvas& canvas, sf::Vector2i lineTop,
                           sf::Color color) const {
  const sf::IntRect box(
      {lineTop.x, lineTop.y + REGULAR_FONT_BASELINE_ROW - CHECK_BOX_SIZE},
      {CHECK_BOX_SIZE, CHECK_BOX_SIZE});
  const int right = box.position.x + box.size.x - BORDER_THICKNESS;
  const int bottom = box.position.y + box.size.y - BORDER_THICKNESS;
  canvas.fill({box.position, {box.size.x, BORDER_THICKNESS}}, color);
  canvas.fill({{box.position.x, bottom}, {box.size.x, BORDER_THICKNESS}},
              color);
  canvas.fill({box.position, {BORDER_THICKNESS, box.size.y}}, color);
  canvas.fill({{right, box.position.y}, {BORDER_THICKNESS, box.size.y}}, color);
  if (mark_ == CheckMark::Checked) {
    const sf::Vector2i margin(CHECK_MARK_MARGIN, CHECK_MARK_MARGIN);
    canvas.fill({box.position + margin, box.size - (margin * SIDES_PER_AXIS)},
                color);
  }
}

}  // namespace rtype::client
