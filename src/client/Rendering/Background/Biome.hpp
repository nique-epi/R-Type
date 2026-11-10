#pragma once

#include <span>
#include <string_view>
#include "BiomeLayer.hpp"

namespace rtype::client {

/** @brief A biome the server may name: its name, and the layers of its
 * decor, from the farthest to the nearest. */
struct Biome {
  std::string_view name;
  std::span<const BiomeLayer> layers;
};

}  // namespace rtype::client
