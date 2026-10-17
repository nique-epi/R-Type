#include "CheckBox.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include "CheckMark.hpp"
#include "IInterfaceCanvas.hpp"
#include "Outline.hpp"
#include "WidgetConstants.hpp"

namespace rtype::client {

CheckBox::CheckBox(sf::Vector2i topLeft, CheckMark mark)
    : topLeft_(topLeft), mark_(mark) {}

void CheckBox::paint(IInterfaceCanvas& canvas, sf::Color color) const {
  const sf::IntRect box(topLeft_, {CHECK_BOX_SIZE, CHECK_BOX_SIZE});
  Outline(box, mark_ == CheckMark::Unavailable ? Outline::Line::Dashed
                                               : Outline::Line::Solid)
      .paint(canvas, color);
  if (mark_ == CheckMark::Checked) {
    const sf::Vector2i margin(CHECK_MARK_MARGIN, CHECK_MARK_MARGIN);
    canvas.fill({box.position + margin, box.size - (margin * SIDES_PER_AXIS)},
                color);
  }
}

}  // namespace rtype::client
