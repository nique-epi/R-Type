#pragma once

#include <string_view>

namespace rtype::client {

/**
 * @brief One layer of the decor of a biome: the asset id of its picture, as
 * tall as the sky and repeated side by side, and the share of the scroll of
 * the level it moves by, so a far layer moves less than a near one.
 */
struct BiomeLayer {
  std::string_view image;
  float scrollFactor{1.0F};
};

}  // namespace rtype::client
