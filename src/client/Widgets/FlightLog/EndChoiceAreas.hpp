#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @brief Where the choices of the flight log lie, in pixels of the
 * interface picture: the QUIT button, where the call to play again starts,
 * and where the pointer presses it. */
struct EndChoiceAreas {
  sf::IntRect quit;
  sf::Vector2i promptTopLeft;
  sf::IntRect prompt;
};

}  // namespace rtype::client
