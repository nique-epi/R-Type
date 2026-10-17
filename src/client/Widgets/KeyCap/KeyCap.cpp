#include "KeyCap.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <string>
#include <utility>
#include "IInterfaceCanvas.hpp"
#include "TextStyle.hpp"
#include "WidgetColors.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

namespace rtype::client {

KeyCap::KeyCap(std::string name, Edge edge)
    : name_(std::move(name)), edge_(edge) {}

int KeyCap::width() const {
  return std::max(KEY_CAP_MINIMUM_WIDTH,
                  regularTextWidth(name_) + (SIDES_PER_AXIS * KEY_CAP_PADDING));
}

void KeyCap::paint(IInterfaceCanvas& canvas, sf::Vector2i topLeft) const {
  sf::Color edge = KEY_CAP_EDGE_COLOR;
  if (edge_ == Edge::Selected) {
    edge = SELECTED_KEY_CAP_EDGE_COLOR;
  } else if (edge_ == Edge::Conflict) {
    edge = CONFLICT_KEY_CAP_EDGE_COLOR;
  }
  const sf::Vector2i size(width(), CONTROL_HEIGHT);
  const sf::Vector2i border(BORDER_THICKNESS, BORDER_THICKNESS);
  canvas.fill({topLeft, size}, edge);
  canvas.fill({{topLeft.x, topLeft.y + size.y - KEY_CAP_SIDE_THICKNESS},
               {size.x, KEY_CAP_SIDE_THICKNESS}},
              KEY_CAP_SIDE_COLOR);
  canvas.fill({topLeft + border,
               {size.x - (SIDES_PER_AXIS * BORDER_THICKNESS),
                size.y - BORDER_THICKNESS - KEY_CAP_SIDE_THICKNESS}},
              KEY_CAP_FACE_COLOR);
  canvas.write(
      name_,
      {centeredTextLeft({topLeft, size}, name_), topLeft.y + CONTROL_TEXT_TOP},
      {.font = TextFont::Regular, .color = KEY_CAP_TEXT_COLOR});
}

}  // namespace rtype::client
