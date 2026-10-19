#pragma once

namespace rtype::game {

/**
 * @brief Where the ships of the players appear, across the playfield: the
 * share of its width between its left edge and the center of a new ship.
 */
constexpr float SHIP_START_X_RATIO = 0.1F;

/**
 * @brief How far past the right edge of the playfield the center of a new Bydo
 * appears, in logical units, so the whole Bydo is off screen.
 *
 * Provisional: it is half a Bydo width, 64 units, until the size of a Bydo is
 * fixed.
 */
constexpr float BYDO_SPAWN_MARGIN = 64.0F;

/**
 * @brief How far past the left edge of the playfield the center of a Bydo must
 * be before it is removed, in logical units, so the whole Bydo is off screen.
 *
 * Provisional, like BYDO_SPAWN_MARGIN: it is half a Bydo width, 64 units, until
 * the size of a Bydo is fixed.
 */
constexpr float BYDO_EXIT_MARGIN = 64.0F;

/**
 * @brief How far from the top and from the bottom of the playfield the center
 * of a new Bydo stays, in logical units, so its whole height is on screen.
 *
 * Provisional: it is half a Bydo height, 64 units, until the size of a Bydo is
 * fixed.
 */
constexpr float BYDO_SPAWN_VERTICAL_MARGIN = 64.0F;

}  // namespace rtype::game
