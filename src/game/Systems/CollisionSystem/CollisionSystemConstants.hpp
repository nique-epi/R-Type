#pragma once

#include "Bounds.hpp"
#include "CollisionConstants.hpp"
#include "PlayfieldConstants.hpp"

namespace rtype::game {

/**
 * @brief Side of a cell of the grid CollisionSystem sorts the boxes into, in
 * logical units: the playfield is 16 cells wide and 9 tall. A box wider or
 * taller than a cell is counted in every cell it covers.
 */
constexpr float COLLISION_CELL_SIDE = 120.0F;

/**
 * @brief The area the grid of CollisionSystem covers: the whole playfield. A
 * box beyond it is counted in the cells along the edge it lies beyond.
 */
constexpr engine::Bounds COLLISION_GRID_AREA{
    .centerX = PLAYFIELD_WIDTH * engine::HALF,
    .centerY = PLAYFIELD_HEIGHT * engine::HALF,
    .width = PLAYFIELD_WIDTH,
    .height = PLAYFIELD_HEIGHT};

}  // namespace rtype::game
