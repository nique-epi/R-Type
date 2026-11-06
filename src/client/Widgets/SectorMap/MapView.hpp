#pragma once

namespace rtype::client {

/** @brief How the map is seen: its zoom, and where the centre of the planet
 * lands on the grid of the interface, which may lie outside the map. */
struct MapView {
  double zoom{1.0};
  double centerX{};
  double centerY{};

  bool operator==(const MapView&) const = default;
};

}  // namespace rtype::client
