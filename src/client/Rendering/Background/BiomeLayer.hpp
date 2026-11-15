#pragma once

#include <string_view>

namespace rtype::client {

/**
 * @brief One layer of the decor of a biome: the tile layer of its LDtk
 * project that holds it, the asset id of the tileset its tiles come from, and
 * the share of the scroll of the level it moves by, so a far layer moves less
 * than a near one.
 */
struct BiomeLayer {
  std::string_view layer;
  std::string_view tileset;
  float scrollFactor{1.0F};
};

}  // namespace rtype::client
