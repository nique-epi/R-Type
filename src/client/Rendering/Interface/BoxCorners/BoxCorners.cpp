#include "BoxCorners.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include "IInterfaceCanvas.hpp"
#include "InterfaceConstants.hpp"

namespace rtype::client {

void paintBoxCorners(IInterfaceCanvas& canvas, sf::IntRect box,
                     sf::Color color) {
  const int left = box.position.x;
  const int right = box.position.x + box.size.x - 1;
  const int upper = box.position.y;
  const int lower = box.position.y + std::max(0, box.size.y - 1);
  const int farEnd = BOX_CORNER_LENGTH - 1;
  const sf::Vector2i across(BOX_CORNER_LENGTH, 1);
  const sf::Vector2i down(1, BOX_CORNER_LENGTH);
  canvas.fill({{left, upper}, across}, color);
  canvas.fill({{left, upper}, down}, color);
  canvas.fill({{right - farEnd, upper}, across}, color);
  canvas.fill({{right, upper}, down}, color);
  canvas.fill({{left, lower}, across}, color);
  canvas.fill({{left, lower - farEnd}, down}, color);
  canvas.fill({{right - farEnd, lower}, across}, color);
  canvas.fill({{right, lower - farEnd}, down}, color);
}

}  // namespace rtype::client
