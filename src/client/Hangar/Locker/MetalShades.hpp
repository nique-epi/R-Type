#pragma once

#include <SFML/Graphics/Color.hpp>

namespace rtype::client {

/** @brief The two shades a metal face is worn into: its light and its dark.
 */
struct MetalShades {
  sf::Color light;
  sf::Color dark;
};

}  // namespace rtype::client
