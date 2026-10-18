#include "Button.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include "BarState.hpp"
#include "CutCornerBox.hpp"
#include "IInterfaceCanvas.hpp"
#include "InterfaceConstants.hpp"
#include "RowControl.hpp"
#include "SunkBox.hpp"
#include "TextField.hpp"
#include "TextStyle.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

Button::Button(Text text, sf::IntRect area, std::function<void()> action,
               std::optional<TextField> field)
    : text_(std::move(text)),
      area_(area),
      action_(std::move(action)),
      field_(std::move(field)) {}

Button::Button(Text text, sf::IntRect area, std::unique_ptr<RowControl> control)
    : text_(std::move(text)), area_(area), control_(std::move(control)) {}

bool Button::isEnabled() const { return enabled_; }

void Button::setEnabled(bool enabled) { enabled_ = enabled; }

void Button::setDescription(std::string description) {
  text_.description = std::move(description);
}

bool Button::contains(sf::Vector2i pixel) const {
  return area_.contains(pixel);
}

const std::string& Button::description() const { return text_.description; }

TextField* Button::field() { return field_.has_value() ? &*field_ : nullptr; }

const TextField* Button::field() const {
  return field_.has_value() ? &*field_ : nullptr;
}

bool Button::isActivatable() const {
  return control_ == nullptr || control_->isActivatable();
}

RowControl* Button::control() { return control_.get(); }

const RowControl* Button::control() const { return control_.get(); }

sf::IntRect Button::controlArea() const {
  if (control_ == nullptr) {
    return {};
  }
  const int width = control_->width();
  return {{area_.position.x + area_.size.x - ROW_PADDING - width,
           area_.position.y + ROW_CONTROL_TOP},
          {width, CONTROL_HEIGHT}};
}

void Button::activate() {
  if (control_ != nullptr) {
    control_->activate();
    return;
  }
  if (action_ != nullptr) {
    action_();
  }
}

void Button::paint(IInterfaceCanvas& canvas, BarState state) const {
  if (control_ != nullptr) {
    paintRow(canvas, state);
    return;
  }
  if (field_.has_value()) {
    paintField(canvas, state, *field_);
    return;
  }
  const Look look = lookOf(state);
  CutCornerBox(area_, look.bar).paint(canvas);
  canvas.write(
      text_.label,
      area_.position + BUTTON_TEXT_OFFSET + sf::Vector2i(0, look.textDrop),
      {.font = TextFont::Regular, .color = look.text});
}

void Button::paintOverlay(IInterfaceCanvas& canvas) const {
  if (control_ != nullptr) {
    control_->paintOverlay(canvas, controlArea());
  }
}

void Button::paintRow(IInterfaceCanvas& canvas, BarState state) const {
  const Look look = lookOf(state);
  if (state == BarState::Selected || state == BarState::Pressed) {
    CutCornerBox(area_, look.bar).paint(canvas);
  }
  canvas.write(
      text_.label,
      area_.position + ROW_LABEL_OFFSET + sf::Vector2i(0, look.textDrop),
      {.font = TextFont::Regular, .color = look.text});
  control_->paint(canvas, controlArea(), state);
}

void Button::paintField(IInterfaceCanvas& canvas, BarState state,
                        const TextField& field) const {
  const FieldLook look = fieldLookOf(state);
  canvas.write(text_.label, area_.position,
               {.font = TextFont::Small, .color = look.label});
  const int labelHeight = TEXT_LINE_HEIGHT + FIELD_LABEL_GAP;
  const sf::IntRect box({area_.position.x, area_.position.y + labelHeight},
                        {area_.size.x, area_.size.y - labelHeight});
  SunkBox(box, look.sides).paint(canvas);
  const std::string& text = field.text();
  const sf::Vector2i textAnchor =
      box.position + sf::Vector2i(FIELD_TEXT_PADDING, FIELD_TEXT_PADDING);
  canvas.write(text, textAnchor,
               {.font = TextFont::Regular, .color = look.text});
  if (look.caret) {
    canvas.fill({{textAnchor.x + regularTextWidth(text),
                  textAnchor.y - FIELD_CARET_OVERHANG},
                 FIELD_CARET_SIZE},
                FIELD_CARET_COLOR);
  }
}

Button::Look Button::lookOf(BarState state) {
  const sf::Color text = barTextColor(state);
  switch (state) {
    case BarState::Selected:
      return {.bar = {.border = SELECTED_BAR_BORDER_COLOR,
                      .inside = SELECTED_BAR_INSIDE_COLOR},
              .text = text,
              .textDrop = 0};
    case BarState::Pressed:
      return {.bar = {.border = SELECTED_BAR_BORDER_COLOR,
                      .inside = PRESSED_BAR_INSIDE_COLOR},
              .text = text,
              .textDrop = PRESSED_TEXT_DROP};
    case BarState::Disabled:
      return {.bar = {.border = DISABLED_BAR_BORDER_COLOR,
                      .inside = BAR_INSIDE_COLOR},
              .text = text,
              .textDrop = 0};
    case BarState::Idle:
      break;
  }
  return {.bar = {.border = BAR_BORDER_COLOR, .inside = BAR_INSIDE_COLOR},
          .text = text,
          .textDrop = 0};
}

Button::FieldLook Button::fieldLookOf(BarState state) {
  switch (state) {
    case BarState::Selected:
    case BarState::Pressed:
      return {.label = SELECTED_FIELD_LABEL_COLOR,
              .sides = SunkBox::Sides::Lit,
              .text = SELECTED_FIELD_TEXT_COLOR,
              .caret = true};
    case BarState::Disabled:
      return {.label = DISABLED_BAR_TEXT_COLOR,
              .sides = SunkBox::Sides::Shaded,
              .text = DISABLED_BAR_TEXT_COLOR,
              .caret = false};
    case BarState::Idle:
      break;
  }
  return {.label = FIELD_TEXT_COLOR,
          .sides = SunkBox::Sides::Shaded,
          .text = FIELD_TEXT_COLOR,
          .caret = false};
}

}  // namespace rtype::client
