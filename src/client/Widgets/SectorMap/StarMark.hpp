#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include "StarLook.hpp"

namespace rtype::client {

/** @brief A server on the map: the pixel it stands on, its look, and how far
 * its planet has turned, in frames (see PLANET_TURN_FRAME_COUNT). */
struct StarMark {
  sf::Vector2i point;
  StarLook look;
  std::size_t turn{};
};

}  // namespace rtype::client
