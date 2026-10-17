#include "CutCornerBox.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "IInterfaceCanvas.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/**
 * @brief Fills an area whose top-left corner is cut, and its bottom-right
 * corner too when it is closed: its first row starts cutRows pixels in, each
 * next row one pixel less, and the same from its last row on the right.
 */
void fillCutArea(IInterfaceCanvas& canvas, sf::IntRect area, int cutRows,
                 CutCornerBox::Shape shape, sf::Color color) {
  const bool closed = shape == CutCornerBox::Shape::Closed;
  const int bottomRow = area.position.y + area.size.y - 1;
  for (int row = 0; row < cutRows; ++row) {
    const int cut = cutRows - row;
    canvas.fill({{area.position.x + cut, area.position.y + row},
                 {area.size.x - cut, 1}},
                color);
    if (closed) {
      canvas.fill({{area.position.x, bottomRow - row}, {area.size.x - cut, 1}},
                  color);
    }
  }
  const int bottomCutRows = closed ? cutRows : 0;
  canvas.fill({{area.position.x, area.position.y + cutRows},
               {area.size.x, area.size.y - cutRows - bottomCutRows}},
              color);
}

}  // namespace

CutCornerBox::CutCornerBox(sf::IntRect area, Colors colors)
    : area_(area), colors_(colors) {}

CutCornerBox::CutCornerBox(sf::IntRect area, Colors colors, Shape shape)
    : area_(area), colors_(colors), shape_(shape) {}

void CutCornerBox::paint(IInterfaceCanvas& canvas) const {
  fillCutArea(canvas, area_, CORNER_CUT_ROWS, shape_, colors_.border);
  const int bottomBorder = shape_ == Shape::Closed ? BORDER_THICKNESS : 0;
  const sf::IntRect inside(
      area_.position + sf::Vector2i(BORDER_THICKNESS, BORDER_THICKNESS),
      {area_.size.x - (SIDES_PER_AXIS * BORDER_THICKNESS),
       area_.size.y - BORDER_THICKNESS - bottomBorder});
  fillCutArea(canvas, inside, CORNER_CUT_ROWS - BORDER_THICKNESS, shape_,
              colors_.inside);
}

}  // namespace rtype::client
