#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @brief One tile of a map: where it lies in the map and where its picture
 * lies in its tileset, both in pixels, and whether it is mirrored. */
struct MapTile {
  sf::Vector2i position;
  sf::Vector2i source;
  bool flipX{false};
  bool flipY{false};
};

}  // namespace rtype::client
