#pragma once

#include <SFML/Graphics/Color.hpp>

namespace rtype::client {

/** @brief A color of the interface, and the one HIGH CONTRAST draws in its
 * place. */
struct ColorReplacement {
  sf::Color from;
  sf::Color to;
};

}  // namespace rtype::client
