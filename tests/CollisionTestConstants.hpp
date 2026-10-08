#pragma once

/**
 * @brief Width and height of the square every collision test starts from.
 */
constexpr float SQUARE_SIDE = 10.0F;

/**
 * @brief Distance between two square centers that leaves the squares sharing
 * some area, on one axis.
 */
constexpr float CENTER_DISTANCE_OVERLAPPING = 9.0F;

/**
 * @brief Distance between two square centers that makes the squares touch
 * along an edge without sharing area, on one axis.
 */
constexpr float CENTER_DISTANCE_TOUCHING = 10.0F;

/**
 * @brief Distance between two square centers that leaves a gap between the
 * squares, on one axis.
 */
constexpr float CENTER_DISTANCE_APART = 11.0F;

/**
 * @brief Side of a square small enough to sit strictly inside the starting
 * square.
 */
constexpr float SMALL_SQUARE_SIDE = 2.0F;

/**
 * @brief Side of a rectangle that has no area.
 */
constexpr float EMPTY_SIDE = 0.0F;

/**
 * @brief Distance from the center of the starting square to one of its edges.
 */
constexpr float EDGE_OFFSET = 5.0F;
