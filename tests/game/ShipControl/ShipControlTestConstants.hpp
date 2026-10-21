#pragma once

#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "ShipControlConstants.hpp"

namespace rtype::game::testing {

/** @brief The middle of the playfield, where a test ship starts when it must
 * meet no edge. */
constexpr Position PLAYFIELD_CENTER{.x = PLAYFIELD_WIDTH / 2.0F,
                                    .y = PLAYFIELD_HEIGHT / 2.0F};

/** @brief How far from the edge it heads for a test ship starts, in logical
 * units. */
constexpr float NEAR_AN_EDGE = 100.0F;

/**
 * @brief How far two positions computed in floats may differ, in logical units:
 * a hundredth of a unit, a four-hundredth of a pixel of a sprite.
 */
constexpr float POSITION_TOLERANCE = 0.01F;

/**
 * @brief Where the center of a ship stops against each edge of the playfield:
 * half a ship away from it, so the whole ship stays on screen.
 */
constexpr float LEFT_EDGE_STOP_X = SHIP_WIDTH / 2.0F;
constexpr float RIGHT_EDGE_STOP_X = PLAYFIELD_WIDTH - (SHIP_WIDTH / 2.0F);
constexpr float TOP_EDGE_STOP_Y = SHIP_HEIGHT / 2.0F;
constexpr float BOTTOM_EDGE_STOP_Y = PLAYFIELD_HEIGHT - (SHIP_HEIGHT / 2.0F);

static_assert(SHIP_SPEED < PLAYFIELD_CENTER.y - TOP_EDGE_STOP_Y,
              "a second of movement from PLAYFIELD_CENTER meets no edge, so a "
              "test of a direction sees the whole distance");
static_assert(NEAR_AN_EDGE < SHIP_SPEED,
              "a second of movement from NEAR_AN_EDGE takes a ship past the "
              "edge, so a test of an edge sees it stop");

}  // namespace rtype::game::testing
