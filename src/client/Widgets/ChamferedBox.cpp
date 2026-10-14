#include "ChamferedBox.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "IInterfaceCanvas.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

namespace {

/**
 * @brief Fills an area whose top-left and bottom-right corners are cut: its
 * first row starts cutRows pixels in, each next row one pixel less, and the
 * same from its last row on the right.
 */
void fillCutArea(IInterfaceCanvas& canvas, sf::IntRect area, int cutRows,
                 sf::Color color) {
  const int bottomRow = area.position.y + area.size.y - 1;
  for (int row = 0; row < cutRows; ++row) {
    const int cut = cutRows - row;
    canvas.fill({{area.position.x + cut, area.position.y + row},
                 {area.size.x - cut, 1}},
                color);
    canvas.fill({{area.position.x, bottomRow - row}, {area.size.x - cut, 1}},
                color);
  }
  canvas.fill({{area.position.x, area.position.y + cutRows},
               {area.size.x, area.size.y - (cutRows * SIDES_PER_AXIS)}},
              color);
}

}  // namespace

void paintChamferedBox(IInterfaceCanvas& canvas, sf::IntRect area,
                       ChamferedBoxColors colors) {
  fillCutArea(canvas, area, CHAMFER_ROWS, colors.border);
  const sf::Vector2i border(BORDER_THICKNESS, BORDER_THICKNESS);
  fillCutArea(canvas,
              {area.position + border, area.size - (border * SIDES_PER_AXIS)},
              CHAMFER_ROWS - BORDER_THICKNESS, colors.inside);
}

}  // namespace rtype::client
