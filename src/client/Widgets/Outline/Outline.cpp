#include "Outline.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "IInterfaceCanvas.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

Outline::Outline(sf::IntRect area, Line line) : area_(area), line_(line) {}

void Outline::paint(IInterfaceCanvas& canvas, sf::Color color) const {
  const int left = area_.position.x;
  const int top = area_.position.y;
  const int right = left + area_.size.x - BORDER_THICKNESS;
  const int bottom = top + area_.size.y - BORDER_THICKNESS;
  if (line_ == Line::Solid) {
    canvas.fill({{left, top}, {area_.size.x, BORDER_THICKNESS}}, color);
    canvas.fill({{left, top}, {BORDER_THICKNESS, area_.size.y}}, color);
    canvas.fill({{left, bottom}, {area_.size.x, BORDER_THICKNESS}}, color);
    canvas.fill({{right, top}, {BORDER_THICKNESS, area_.size.y}}, color);
    return;
  }
  const sf::Vector2i dot(BORDER_THICKNESS, BORDER_THICKNESS);
  for (int column = left; column <= right; column += DASH_PERIOD) {
    canvas.fill({{column, top}, dot}, color);
    canvas.fill({{column, bottom}, dot}, color);
  }
  for (int row = top; row <= bottom; row += DASH_PERIOD) {
    canvas.fill({{left, row}, dot}, color);
    canvas.fill({{right, row}, dot}, color);
  }
}

}  // namespace rtype::client
