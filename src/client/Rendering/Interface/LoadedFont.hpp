#pragma once

#include <SFML/Graphics/Font.hpp>

namespace rtype::client {

/** @brief A font of the interface, and the row of its baseline below the
 * top of a line. */
struct LoadedFont {
  const sf::Font* font{};
  int baselineRow{};
};

}  // namespace rtype::client
