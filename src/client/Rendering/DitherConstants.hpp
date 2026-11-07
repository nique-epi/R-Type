#pragma once

#include <array>

namespace rtype::client {

/** @brief The 4 x 4 Bayer matrix of ordered dither, its side and its levels.
 */
inline constexpr std::array<std::array<int, 4>, 4> BAYER_MATRIX{
    {{0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}}};
inline constexpr int BAYER_SIZE = 4;
inline constexpr double BAYER_LEVELS = 16.0;
/** @brief Half a level of the matrix: a cell's threshold is read at its
 * centre, and a tone is centred on it. */
inline constexpr double BAYER_CELL_CENTRE = 0.5;

}  // namespace rtype::client
