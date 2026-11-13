#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderStates.hpp>

namespace rtype::client {

/** @brief What the outline of a line takes from it: the color whose opacity
 * it keeps, and the states its letters are drawn with. */
struct OutlinedText {
  sf::Color color;
  sf::RenderStates states;
};

}  // namespace rtype::client
