#include "Button.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Utf.hpp>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include "ChamferedBox.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceConstants.hpp"
#include "TextField.hpp"
#include "TextStyle.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

Button::Button(Text text, sf::IntRect area, std::function<void()> action,
               std::optional<TextField> field)
    : text_(std::move(text)),
      area_(area),
      action_(std::move(action)),
      field_(std::move(field)) {}

bool Button::isEnabled() const { return enabled_; }

void Button::setEnabled(bool enabled) { enabled_ = enabled; }

void Button::setMark(CheckMark mark) { mark_ = mark; }

bool Button::contains(sf::Vector2i pixel) const {
  return area_.contains(pixel);
}

const std::string& Button::description() const { return text_.description; }

TextField* Button::field() { return field_.has_value() ? &*field_ : nullptr; }

const TextField* Button::field() const {
  return field_.has_value() ? &*field_ : nullptr;
}

void Button::activate() const {
  if (action_ != nullptr) {
    action_();
  }
}

void Button::paint(IInterfaceCanvas& canvas, State state) const {
  if (field_.has_value()) {
    paintField(canvas, state, *field_);
    return;
  }
  const Look look = lookOf(state);
  ChamferedBox(area_, look.bar).paint(canvas);
  sf::Vector2i labelAnchor =
      area_.position + BUTTON_TEXT_OFFSET + sf::Vector2i(0, look.textDrop);
  if (mark_ != CheckMark::None) {
    paintCheckBox(canvas, labelAnchor, look.text);
    labelAnchor.x += CHECK_BOX_SIZE + CHECK_BOX_GAP;
  }
  canvas.write(text_.label, labelAnchor,
               {.font = TextFont::Regular, .color = look.text});
}

void Button::paintField(IInterfaceCanvas& canvas, State state,
                        const TextField& field) const {
  const FieldLook look = fieldLookOf(state);
  canvas.write(text_.label, area_.position,
               {.font = TextFont::Small, .color = look.label});
  const int labelHeight = TEXT_LINE_HEIGHT + FIELD_LABEL_GAP;
  const sf::IntRect box({area_.position.x, area_.position.y + labelHeight},
                        {area_.size.x, area_.size.y - labelHeight});
  canvas.fill(box, FIELD_INSIDE_COLOR);
  paintSides(canvas, box, look.sides);
  const std::string& text = field.text();
  const sf::Vector2i textAnchor =
      box.position + sf::Vector2i(FIELD_TEXT_PADDING, FIELD_TEXT_PADDING);
  canvas.write(text, textAnchor,
               {.font = TextFont::Regular, .color = look.text});
  if (look.caret) {
    const int characters =
        static_cast<int>(sf::Utf8::count(text.begin(), text.end()));
    canvas.fill({{textAnchor.x + (characters * REGULAR_FONT_ADVANCE),
                  textAnchor.y - FIELD_CARET_OVERHANG},
                 FIELD_CARET_SIZE},
                FIELD_CARET_COLOR);
  }
}

Button::Look Button::lookOf(State state) {
  switch (state) {
    case State::Selected:
      return {.bar = {.border = SELECTED_BAR_BORDER_COLOR,
                      .inside = SELECTED_BAR_INSIDE_COLOR},
              .text = SELECTED_BAR_TEXT_COLOR,
              .textDrop = 0};
    case State::Pressed:
      return {.bar = {.border = SELECTED_BAR_BORDER_COLOR,
                      .inside = PRESSED_BAR_INSIDE_COLOR},
              .text = PRESSED_BAR_TEXT_COLOR,
              .textDrop = PRESSED_TEXT_DROP};
    case State::Disabled:
      return {.bar = {.border = DISABLED_BAR_BORDER_COLOR,
                      .inside = BAR_INSIDE_COLOR},
              .text = DISABLED_BAR_TEXT_COLOR,
              .textDrop = 0};
    case State::Idle:
      break;
  }
  return {.bar = {.border = BAR_BORDER_COLOR, .inside = BAR_INSIDE_COLOR},
          .text = BAR_TEXT_COLOR,
          .textDrop = 0};
}

Button::FieldLook Button::fieldLookOf(State state) {
  switch (state) {
    case State::Selected:
    case State::Pressed:
      return {.label = SELECTED_FIELD_LABEL_COLOR,
              .sides = {.topAndLeft = SELECTED_FIELD_BORDER_COLOR,
                        .bottomAndRight = SELECTED_FIELD_BORDER_COLOR},
              .text = SELECTED_FIELD_TEXT_COLOR,
              .caret = true};
    case State::Disabled:
      return {.label = DISABLED_BAR_TEXT_COLOR,
              .sides = {.topAndLeft = FIELD_SHADED_SIDE_COLOR,
                        .bottomAndRight = FIELD_LIT_SIDE_COLOR},
              .text = DISABLED_BAR_TEXT_COLOR,
              .caret = false};
    case State::Idle:
      break;
  }
  return {.label = FIELD_TEXT_COLOR,
          .sides = {.topAndLeft = FIELD_SHADED_SIDE_COLOR,
                    .bottomAndRight = FIELD_LIT_SIDE_COLOR},
          .text = FIELD_TEXT_COLOR,
          .caret = false};
}

void Button::paintSides(IInterfaceCanvas& canvas, sf::IntRect box,
                        SideColors colors) {
  const int right = box.position.x + box.size.x - BORDER_THICKNESS;
  const int bottom = box.position.y + box.size.y - BORDER_THICKNESS;
  canvas.fill({box.position, {box.size.x, BORDER_THICKNESS}},
              colors.topAndLeft);
  canvas.fill({box.position, {BORDER_THICKNESS, box.size.y}},
              colors.topAndLeft);
  canvas.fill({{box.position.x, bottom}, {box.size.x, BORDER_THICKNESS}},
              colors.bottomAndRight);
  canvas.fill({{right, box.position.y}, {BORDER_THICKNESS, box.size.y}},
              colors.bottomAndRight);
}

void Button::paintCheckBox(IInterfaceCanvas& canvas, sf::Vector2i lineTop,
                           sf::Color color) const {
  const sf::IntRect box(
      {lineTop.x, lineTop.y + REGULAR_FONT_BASELINE_ROW - CHECK_BOX_SIZE},
      {CHECK_BOX_SIZE, CHECK_BOX_SIZE});
  paintSides(canvas, box, {.topAndLeft = color, .bottomAndRight = color});
  if (mark_ == CheckMark::Checked) {
    const sf::Vector2i margin(CHECK_MARK_MARGIN, CHECK_MARK_MARGIN);
    canvas.fill({box.position + margin, box.size - (margin * SIDES_PER_AXIS)},
                color);
  }
}

}  // namespace rtype::client
