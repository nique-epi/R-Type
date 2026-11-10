#pragma once

#include <cstddef>

namespace rtype::engine {

/**
 * @brief Most cells a UniformGrid lines up along one axis.
 *
 * It keeps a mistaken layout, such as a cell side of a thousandth of the area,
 * from asking for memory without bound: at 256 by 256 cells, the two tables of
 * the cells hold 65,537 entries each.
 */
constexpr std::size_t MAXIMUM_GRID_LINES = 256;

}  // namespace rtype::engine
