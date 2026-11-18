#pragma once

#include <cstddef>

namespace rtype::client {

/** @brief Where one copy of a map of a biome is drawn: the map, by its rank
 * in the project, and the column of the sky its left edge lies on, in
 * background pixels. */
struct MapColumn {
  std::size_t map{};
  int column{};

  bool operator==(const MapColumn&) const = default;
};

}  // namespace rtype::client
