#include "GroundLookup.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>
#include "BiomeGround.hpp"
#include "GroundConstants.hpp"
#include "GroundPlace.hpp"
#include "LdtkConstants.hpp"

namespace rtype::game {

namespace {

/** @returns The row of @p rows under @p place, in logical units; nothing
 * where the column has none or lies before the first map. */
std::optional<float> rowAt(const std::vector<int>& rows, GroundPlace place) {
  if (rows.empty()) {
    return std::nullopt;
  }
  const auto screenPixel =
      static_cast<std::int64_t>(std::floor(place.x / LDTK_PIXEL));
  const auto scrolledPixels =
      static_cast<std::int64_t>(std::floor(place.scrolled / LDTK_PIXEL));
  const std::int64_t mapPixel =
      screenPixel - GROUND_ENTRANCE_PIXEL + scrolledPixels;
  if (mapPixel < 0) {
    return std::nullopt;
  }
  const int row = rows[static_cast<std::size_t>(mapPixel) % rows.size()];
  if (row == GROUND_NONE) {
    return std::nullopt;
  }
  return static_cast<float>(row) * LDTK_PIXEL;
}

}  // namespace

std::optional<float> floorRowAt(const BiomeGround& ground, GroundPlace place) {
  return rowAt(ground.floorRows, place);
}

std::optional<float> ceilingRowAt(const BiomeGround& ground,
                                  GroundPlace place) {
  return rowAt(ground.ceilingRows, place);
}

}  // namespace rtype::game
