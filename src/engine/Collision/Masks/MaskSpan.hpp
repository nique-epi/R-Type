#pragma once

namespace rtype::engine {

/** @brief A run of pixels along one row: the row, the column of its first
 * pixel, and how many pixels it covers, from left to right. */
struct MaskSpan {
  int row{};
  int first{};
  int count{};
};

}  // namespace rtype::engine
