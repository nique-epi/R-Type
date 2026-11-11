#pragma once

#include "Bounds.hpp"
#include "CollisionConstants.hpp"
#include "PlayfieldConstants.hpp"

namespace rtype::game {

/**
 * @brief Side of a cell of the grid CollisionSystem sorts the boxes into, in
 * logical units: the playfield is 32 cells wide and 18 tall. A box wider or
 * taller than a cell is counted in every cell it covers.
 *
 * Chosen with script_budget_bench (300 scripted bodies, 1000 bullets, four
 * ships, 3600 ticks, Release, Apple M3 Pro, three runs each): median time of
 * CollisionSystem per tick, 0.161 to 0.164 ms with cells of 60, 0.189 to
 * 0.193 ms with 120, 0.254 to 0.255 ms with 240, the three finding the same
 * collisions. A ship of 128 by 64 covers six to twelve cells of 60; walls were
 * not measured.
 */
constexpr float COLLISION_CELL_SIDE = 60.0F;

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
