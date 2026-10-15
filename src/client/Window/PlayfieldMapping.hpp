#pragma once

#include <SFML/System/Vector2.hpp>
#include <functional>

namespace rtype::client {

/** @brief Gives the point of the playfield, in logical units, under a pixel
 * of the window, as GameWindow::playfieldPointAt() does. */
using PlayfieldMapping = std::function<sf::Vector2f(sf::Vector2i)>;

}  // namespace rtype::client
