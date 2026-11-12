#pragma once

#include <string>
#include <vector>
#include "CollisionFilter.hpp"
#include "DataFileText.hpp"

namespace rtype::game {

/**
 * @brief The keys `layer` and `hits` of a sheet: the collision layer a body
 * lies in and the layers it hits, as the file names them, and the filter they
 * make once every sheet is read.
 */
struct CollisionSheet {
  /** @brief The name of the layer the body lies in. */
  std::string layer;
  /** @brief The names of the layers it hits, in the order of the file, each
   * with where it is written, for the error EntitySheets raises when no sheet
   * lies in it; empty for a body that hits nothing. */
  std::vector<engine::DataFileText> hits;
  /** @brief Both as sets of layers; empty until EntitySheets gave each layer
   * the sheets name its bit. */
  engine::CollisionFilter filter{};
};

}  // namespace rtype::game
