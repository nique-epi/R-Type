#pragma once

namespace rtype::engine {

/** @brief A rectangle of whole cells of the grid masks are laid on: the
 * column and the row of its top-left cell, and how many columns and rows it
 * covers. An area of no column or no row covers no cell. */
struct MaskArea {
  int left{};
  int top{};
  int width{};
  int height{};
};

}  // namespace rtype::engine
