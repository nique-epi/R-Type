#include "Slider.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <functional>
#include <optional>
#include <string>
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

Slider::Slider(Range range, ValueBinding<int> value,
               std::function<bool()> inactive, std::string unit)
    : range_(range),
      value_(std::move(value)),
      inactive_(std::move(inactive)),
      unit_(std::move(unit)) {}

int Slider::width() const {
  return SLIDER_GROOVE_SIZE.x + CONTROL_GAP + SLIDER_VALUE_WIDTH;
}

void Slider::paint(IInterfaceCanvas& canvas, sf::IntRect area,
                   BarState row) const {
  const bool dimmed = inactive_();
  const int value = value_.read();
  const sf::IntRect groove(area.position, SLIDER_GROOVE_SIZE);
  const int thumbCenter = trackPixelOf(value);
  const Look look = lookOf(row, dimmed);
  paintTrack(canvas, groove, thumbCenter, look.fill);
  paintThumb(canvas, groove, thumbCenter, look.thumb);
  writeValue(canvas, area, row,
             dimmed ? std::string(SLIDER_INACTIVE_TEXT)
                    : std::to_string(value) + unit_);
}

bool Slider::holdsInput() const { return dragging_; }

bool Slider::keyPressed(const sf::Event::KeyPressed& key) {
  const std::optional<int> target = valueAfter(key.code);
  if (!target.has_value()) {
    return false;
  }
  moveTo(*target);
  return true;
}

bool Slider::pointerPressed(sf::Vector2i pixel, sf::IntRect area) {
  if (!sf::IntRect(area.position, SLIDER_GROOVE_SIZE).contains(pixel)) {
    return false;
  }
  dragging_ = true;
  moveTo(valueAt(pixel, area));
  return true;
}

void Slider::pointerMoved(sf::Vector2i pixel, sf::IntRect area) {
  if (dragging_) {
    moveTo(valueAt(pixel, area));
  }
}

void Slider::pointerReleased([[maybe_unused]] sf::Vector2i pixel,
                             [[maybe_unused]] sf::IntRect area) {
  dragging_ = false;
}

void Slider::cancelInput() { dragging_ = false; }

void Slider::moveTo(int value) const {
  value_.change(std::clamp(value, range_.minimum, range_.maximum));
}

void Slider::paintTrack(IInterfaceCanvas& canvas, sf::IntRect groove,
                        int thumbCenter, sf::Color fill) {
  SunkBox(groove, SunkBox::Sides::Shaded).paint(canvas);
  const sf::Vector2i trackStart(groove.position.x + SLIDER_TRACK_INSET,
                                groove.position.y + SLIDER_TRACK_TOP);
  canvas.fill({trackStart, {SLIDER_TRACK_LENGTH, SLIDER_TRACK_THICKNESS}},
              SLIDER_TRACK_COLOR);
  canvas.fill({trackStart,
               {thumbCenter - SLIDER_TRACK_INSET + 1, SLIDER_TRACK_THICKNESS}},
              fill);
}

void Slider::paintThumb(IInterfaceCanvas& canvas, sf::IntRect groove,
                        int thumbCenter, sf::Color inside) {
  const sf::IntRect thumbArea(
      {groove.position.x + thumbCenter - (SLIDER_THUMB_SIZE.x / SIDES_PER_AXIS),
       groove.position.y + SLIDER_THUMB_TOP},
      SLIDER_THUMB_SIZE);
  const sf::Vector2i border(BORDER_THICKNESS, BORDER_THICKNESS);
  canvas.fill(thumbArea, SLIDER_THUMB_OUTLINE_COLOR);
  canvas.fill(
      {thumbArea.position + border, thumbArea.size - (border * SIDES_PER_AXIS)},
      inside);
}

void Slider::writeValue(IInterfaceCanvas& canvas, sf::IntRect area,
                        BarState row, const std::string& text) {
  canvas.write(
      text, {area.position.x + area.size.x, area.position.y + CONTROL_TEXT_TOP},
      {.font = TextFont::Regular,
       .color = barTextColor(row),
       .alignment = TextAlignment::Right});
}

Slider::Look Slider::lookOf(BarState row, bool dimmed) {
  if (dimmed) {
    return {.fill = SLIDER_TRACK_COLOR, .thumb = SLIDER_TRACK_COLOR};
  }
  if (row == BarState::Selected) {
    return {.fill = SELECTED_SLIDER_FILL_COLOR,
            .thumb = SELECTED_SLIDER_THUMB_COLOR};
  }
  return {.fill = SLIDER_FILL_COLOR, .thumb = SLIDER_THUMB_COLOR};
}

std::optional<int> Slider::valueAfter(sf::Keyboard::Key key) const {
  const int value = value_.read();
  const int page = range_.step * range_.pageSteps;
  switch (key) {
    case DECREASE_KEY:
      return value - range_.step;
    case INCREASE_KEY:
      return value + range_.step;
    case FIRST_VALUE_KEY:
      return range_.minimum;
    case LAST_VALUE_KEY:
      return range_.maximum;
    case PAGE_UP_KEY:
      return value + page;
    case PAGE_DOWN_KEY:
      return value - page;
    default:
      return std::nullopt;
  }
}

int Slider::trackPixelOf(int value) const {
  const int lastPixel = SLIDER_TRACK_LENGTH - 1;
  return SLIDER_TRACK_INSET + (((value - range_.minimum) * lastPixel) /
                               (range_.maximum - range_.minimum));
}

int Slider::valueAt(sf::Vector2i pixel, sf::IntRect area) const {
  const int lastPixel = SLIDER_TRACK_LENGTH - 1;
  const int offset =
      std::clamp(pixel.x - area.position.x - SLIDER_TRACK_INSET, 0, lastPixel);
  const int stepCount = (range_.maximum - range_.minimum) / range_.step;
  const int nearestStep =
      ((offset * stepCount * ROUNDING_DIVISOR) + lastPixel) /
      (lastPixel * ROUNDING_DIVISOR);
  return range_.minimum + (nearestStep * range_.step);
}

}  // namespace rtype::client
