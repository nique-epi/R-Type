#pragma once

#include <cstdint>
#include "CollisionFilter.hpp"
#include "CollisionFiltering.hpp"
#include "CollisionLayerConstants.hpp"

/**
 * @brief The numbers of two players, one who fires and one whose ship is
 * fired at.
 */
constexpr std::uint8_t FIRING_PLAYER_NUMBER = 1;
constexpr std::uint8_t OTHER_PLAYER_NUMBER = 2;

/**
 * @brief The collision layers of the shipped sheets, one per side, numbered in
 * the order of their names as EntitySheets numbers them, and a layer no
 * shipped sheet lies in.
 */
constexpr std::uint32_t ENEMY_LAYER = rtype::engine::collisionLayerOf(0);
constexpr std::uint32_t ENEMY_MISSILE_LAYER =
    rtype::engine::collisionLayerOf(1);
constexpr std::uint32_t LOOK_ITEM_LAYER = rtype::engine::collisionLayerOf(2);
constexpr std::uint32_t PLAYER_LAYER = rtype::engine::collisionLayerOf(3);
constexpr std::uint32_t PLAYER_MISSILE_LAYER =
    rtype::engine::collisionLayerOf(4);
constexpr std::uint32_t WALL_LAYER = rtype::engine::collisionLayerOf(5);

/**
 * @brief The filter the shipped sheets give a body of each side: an enemy and
 * its missile hit the ships, a missile of a player hits the enemies and, for
 * friendly fire, the ships, a ship hits the look items, which hit nothing.
 */
constexpr rtype::engine::CollisionFilter ENEMY_FILTER{.layer = ENEMY_LAYER,
                                                      .hits = PLAYER_LAYER};
constexpr rtype::engine::CollisionFilter ENEMY_MISSILE_FILTER{
    .layer = ENEMY_MISSILE_LAYER, .hits = PLAYER_LAYER};
constexpr rtype::engine::CollisionFilter LOOK_ITEM_FILTER{
    .layer = LOOK_ITEM_LAYER, .hits = rtype::engine::NO_COLLISION_LAYER};
constexpr rtype::engine::CollisionFilter PLAYER_FILTER{.layer = PLAYER_LAYER,
                                                       .hits = LOOK_ITEM_LAYER};
constexpr rtype::engine::CollisionFilter PLAYER_MISSILE_FILTER{
    .layer = PLAYER_MISSILE_LAYER, .hits = ENEMY_LAYER | PLAYER_LAYER};

/**
 * @brief Filters no shipped sheet has: bodies that hit nothing, an enemy that
 * stops the missiles of the players, missiles of an enemy that hit the
 * enemies or the missiles of the players, and a ship that hits the enemies.
 */
constexpr rtype::engine::CollisionFilter HARMLESS_ENEMY_FILTER{
    .layer = ENEMY_LAYER, .hits = rtype::engine::NO_COLLISION_LAYER};
constexpr rtype::engine::CollisionFilter HARMLESS_ENEMY_MISSILE_FILTER{
    .layer = ENEMY_MISSILE_LAYER, .hits = rtype::engine::NO_COLLISION_LAYER};
constexpr rtype::engine::CollisionFilter HARMLESS_PLAYER_MISSILE_FILTER{
    .layer = PLAYER_MISSILE_LAYER, .hits = rtype::engine::NO_COLLISION_LAYER};
constexpr rtype::engine::CollisionFilter ENEMY_STOPPING_PLAYER_MISSILES_FILTER{
    .layer = ENEMY_LAYER, .hits = PLAYER_MISSILE_LAYER};
constexpr rtype::engine::CollisionFilter ENEMY_MISSILE_HITTING_ENEMIES_FILTER{
    .layer = ENEMY_MISSILE_LAYER, .hits = ENEMY_LAYER};
constexpr rtype::engine::CollisionFilter
    ENEMY_MISSILE_HITTING_PLAYER_MISSILES_FILTER{.layer = ENEMY_MISSILE_LAYER,
                                                 .hits = PLAYER_MISSILE_LAYER};
constexpr rtype::engine::CollisionFilter SHIP_HITTING_ENEMIES_FILTER{
    .layer = PLAYER_LAYER, .hits = ENEMY_LAYER};

/**
 * @brief An enemy that hits the enemies, its own layer.
 */
constexpr rtype::engine::CollisionFilter ENEMY_HITTING_ENEMIES_FILTER{
    .layer = ENEMY_LAYER, .hits = ENEMY_LAYER};

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

/**
 * @brief Distance between two square centers that puts the squares far from
 * each other, on one axis.
 */
constexpr float FAR_DISTANCE = 100.0F;

/**
 * @brief How far to the right of its Position a moved square box is centered:
 * three sides, so it is apart from a square on that Position.
 */
constexpr float MOVED_BOX_OFFSET_X = 30.0F;

/**
 * @brief Side of a square box wider than two cells of the collision grid.
 */
constexpr float WIDE_BOX_SIDE = 300.0F;

/**
 * @brief Centers of two wide boxes 100 units apart on the x axis, both on the
 * same y: from x 50 to 350 and from 150 to 450, they overlap and share several
 * cells of the collision grid.
 */
constexpr float WIDE_BOX_CENTER_X = 200.0F;
constexpr float OTHER_WIDE_BOX_CENTER_X = 300.0F;
constexpr float WIDE_BOX_CENTER_Y = 200.0F;

/**
 * @brief A coordinate beyond the left and the top edges of the playfield.
 */
constexpr float BEYOND_THE_PLAYFIELD = -200.0F;
