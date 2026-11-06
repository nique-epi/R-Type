#pragma once

#include <SFML/System/Vector2.hpp>
#include "StarLook.hpp"

namespace rtype::client {

/** @brief A server on the map: the pixel it stands on, and its look. */
struct StarMark {
  sf::Vector2i point;
  StarLook look;
};

}  // namespace rtype::client
