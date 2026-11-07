#include "FieldTextPainting.hpp"
#include <SFML/System/Vector2.hpp>
#include "FieldText.hpp"
#include "IInterfaceCanvas.hpp"
#include "TextStyle.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

void paintFieldText(IInterfaceCanvas& canvas, const FieldText& field) {
  const sf::Vector2i anchor =
      field.box.position + sf::Vector2i(FIELD_TEXT_PADDING, FIELD_TEXT_PADDING);
  canvas.write(field.text, anchor,
               {.font = TextFont::Regular, .color = field.color});
  if (field.caret) {
    canvas.fill({{anchor.x + regularTextWidth(field.text),
                  anchor.y - FIELD_CARET_OVERHANG},
                 FIELD_CARET_SIZE},
                FIELD_CARET_COLOR);
  }
}

}  // namespace rtype::client
