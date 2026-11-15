#pragma once

#include <SFML/Graphics/Color.hpp>

namespace rtype::client {

/** @brief The four shades of the color of a pocket, red, black or green, as
 * the cloth and the wheel draw it, from the darkest. */
struct PocketShades {
  sf::Color shadow;
  sf::Color dark;
  sf::Color base;
  sf::Color light;
};

}  // namespace rtype::client
