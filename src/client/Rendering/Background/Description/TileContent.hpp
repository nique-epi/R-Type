#pragma once

#include <string>

namespace rtype::client {

/** @brief A layer that shows the tiles of one tile layer of an LDtk project,
 * its maps one after the other: the asset ids of the project and of the
 * tileset its tiles are cut from, and the name of the tile layer. */
struct TileContent {
  std::string project;
  std::string layer;
  std::string tileset;
};

}  // namespace rtype::client
