#pragma once

#include <SFML/Graphics/Color.hpp>

namespace rtype::client {

/** @brief A rectangle of a row of the test card: its width, in twelfths of a
 * bar of the top row, and its color. */
struct TestCardPart {
  int twelfths{};
  sf::Color color;
};

}  // namespace rtype::client
