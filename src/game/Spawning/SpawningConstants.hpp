#pragma once

namespace rtype::game {

/**
 * @brief The way a body faces, as a sign on the horizontal axis: a ship of a
 * player faces right, towards the enemies; an enemy faces left.
 */
constexpr float FACING_RIGHT = 1.0F;
constexpr float FACING_LEFT = -1.0F;

/**
 * @brief Where the ships of the players appear, across the playfield: the
 * share of its width between its left edge and the center of a new ship.
 */
constexpr float SHIP_START_X_RATIO = 0.1F;

/**
 * @brief How far past the right edge of the playfield the center of a new enemy
 * appears, in logical units, so the whole enemy is off screen.
 *
 * Provisional: it is half an enemy width, 64 units, until the size of an enemy
 * is fixed.
 */
constexpr float ENEMY_SPAWN_MARGIN = 64.0F;
/**
 * @brief How far past the left edge of the playfield the center of an enemy
 * must be before it is removed, in logical units, so the whole enemy is off
 * screen.
 *
 * Provisional, like ENEMY_SPAWN_MARGIN: it is half an enemy width, 64 units,
 * until the size of an enemy is fixed.
 */
constexpr float ENEMY_EXIT_MARGIN = 64.0F;

/**
 * @brief Half the height of an enemy, in logical units: how far its center
 * stays from an edge of the area of play for its whole height to be in it.
 *
 * Provisional: 64 units, until the size of an enemy is fixed.
 */
constexpr float ENEMY_SPAWN_VERTICAL_MARGIN = 64.0F;

/**
 * @brief How far above and below its row an enemy may move on its pattern, in
 * logical units; every behaviour stays within it, which
 * WeaveBehaviourConstants.hpp checks.
 *
 * Provisional, like the margins.
 */
constexpr float ENEMY_PATTERN_ROOM = 48.0F;

/**
 * @brief How far from the top of the playfield and from the status band along
 * its bottom the rows of new enemies stay, in logical units: half an enemy and
 * the room of its pattern, so an enemy stays in the area of play even at the
 * top and the bottom of a wave.
 */
constexpr float ENEMY_ROW_MARGIN =
    ENEMY_SPAWN_VERTICAL_MARGIN + ENEMY_PATTERN_ROOM;

}  // namespace rtype::game
