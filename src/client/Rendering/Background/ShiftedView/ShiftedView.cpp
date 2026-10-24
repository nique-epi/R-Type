#include "ShiftedView.hpp"
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>

namespace rtype::client {

sf::View shiftedBy(const sf::View& view, sf::Vector2i pixels) {
  sf::View shifted = view;
  shifted.move(-sf::Vector2f(pixels));
  return shifted;
}

}  // namespace rtype::client
