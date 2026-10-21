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
 * @brief Half the height of a Bydo, in logical units: how far its center stays
 * from an edge of the area of play for its whole height to be in it.
 *
 * Provisional: 64 units, until the size of a Bydo is fixed.
 */
constexpr float BYDO_SPAWN_VERTICAL_MARGIN = 64.0F;

/**
 * @brief How far above and below its row a Bydo may move on its pattern, in
 * logical units; every pattern stays within it, which PatternConstants.hpp
 * checks.
 *
 * Provisional, like the margins.
 */
constexpr float BYDO_PATTERN_ROOM = 48.0F;

/**
 * @brief How far from the top of the playfield and from the status band along
 * its bottom the rows of new Bydos stay, in logical units: half a Bydo and the
 * room of its pattern, so a Bydo stays in the area of play even at the top
 * and the bottom of a wave.
 */
constexpr float BYDO_ROW_MARGIN =
    BYDO_SPAWN_VERTICAL_MARGIN + BYDO_PATTERN_ROOM;

}  // namespace rtype::game
