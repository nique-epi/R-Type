#pragma once

#include <string>
#include "MapLayerKind.hpp"

namespace rtype::client {

/**
 * @brief A layer that shows one layer of an LDtk project, its maps one after
 * the other: the asset ids of the project and of the tileset its pictures
 * are cut from, the name of the layer, and what the layer holds.
 *
 * A layer of tiles shows its tiles. A layer of props shows each entity that
 * has a tile as a sprite, the rectangle its tile names, where the entity
 * lies.
 */
struct MapContent {
  std::string project;
  std::string layer;
  std::string tileset;
  MapLayerKind kind{MapLayerKind::Tiles};
};

}  // namespace rtype::client
