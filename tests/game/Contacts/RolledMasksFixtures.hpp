#pragma once

#include "PixelMask.hpp"

namespace rtype::game::testing {

/** @returns A mask @p side pixels a side with one pixel drawn per row, on
 * its diagonal: row r has its column r alone drawn, so a row of a rolled mask
 * tells which row of it it copies. */
[[nodiscard]] engine::PixelMask diagonalMask(int side);

}  // namespace rtype::game::testing
