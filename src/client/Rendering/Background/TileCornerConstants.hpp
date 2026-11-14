#pragma once

#include <array>
#include <cstddef>

namespace rtype::client {

/** @brief The corners of a square tile, clockwise from the top left. */
constexpr std::size_t TOP_LEFT_CORNER = 0;
constexpr std::size_t TOP_RIGHT_CORNER = 1;
constexpr std::size_t BOTTOM_RIGHT_CORNER = 2;
constexpr std::size_t BOTTOM_LEFT_CORNER = 3;
constexpr std::size_t TILE_CORNERS = 4;

/** @brief The corners of the two triangles a tile is drawn with. */
constexpr std::array TILE_TRIANGLE_CORNERS{
    TOP_LEFT_CORNER, TOP_RIGHT_CORNER,    BOTTOM_RIGHT_CORNER,
    TOP_LEFT_CORNER, BOTTOM_RIGHT_CORNER, BOTTOM_LEFT_CORNER};

}  // namespace rtype::client
