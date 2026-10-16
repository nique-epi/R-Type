#include "SunkBox.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include "IInterfaceCanvas.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

SunkBox::SunkBox(sf::IntRect area, Sides sides) : area_(area), sides_(sides) {}

void SunkBox::paint(IInterfaceCanvas& canvas) const {
  const bool lit = sides_ == Sides::Lit;
  const sf::Color topAndLeft =
      lit ? SELECTED_FIELD_BORDER_COLOR : FIELD_SHADED_SIDE_COLOR;
  const sf::Color bottomAndRight =
      lit ? SELECTED_FIELD_BORDER_COLOR : FIELD_LIT_SIDE_COLOR;
  const int right = area_.position.x + area_.size.x - BORDER_THICKNESS;
  const int bottom = area_.position.y + area_.size.y - BORDER_THICKNESS;
  canvas.fill(area_, FIELD_INSIDE_COLOR);
  canvas.fill({area_.position, {area_.size.x, BORDER_THICKNESS}}, topAndLeft);
  canvas.fill({area_.position, {BORDER_THICKNESS, area_.size.y}}, topAndLeft);
  canvas.fill({{area_.position.x, bottom}, {area_.size.x, BORDER_THICKNESS}},
              bottomAndRight);
  canvas.fill({{right, area_.position.y}, {BORDER_THICKNESS, area_.size.y}},
              bottomAndRight);
}

}  // namespace rtype::client
