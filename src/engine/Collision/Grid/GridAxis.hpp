#pragma once

#include <cstddef>

namespace rtype::engine {

/**
 * @brief One axis of a UniformGrid: where its first cell starts, the side of a
 * cell, and how many cells line up along it, in the units of the boxes.
 */
struct GridAxis {
  float start{};
  float cellSide{};
  std::size_t lineCount{};
};

}  // namespace rtype::engine
