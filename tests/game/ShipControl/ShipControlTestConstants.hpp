#pragma once

#include <chrono>
#include "CollisionConstants.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "ShipControlConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

/** @brief The middle of the play area, where a test ship starts when it must
 * meet no edge. */
constexpr Position PLAY_AREA_CENTER{.x = PLAYFIELD_WIDTH * engine::HALF,
                                    .y = PLAY_AREA_HEIGHT * engine::HALF};

/**
 * @brief How long a test ship moves from PLAY_AREA_CENTER along one axis: half
 * a second, since a second of movement up or down would cross an edge of the
 * play area.
 */
constexpr engine::Duration UNOBSTRUCTED_MOVE_DURATION =
    std::chrono::milliseconds(500);

/** @brief How far from the edge it heads for a test ship starts, in logical
 * units. */
constexpr float NEAR_AN_EDGE = 100.0F;

/**
 * @brief How far two positions computed in floats may differ, in logical units:
 * a hundredth of a unit, a four-hundredth of a pixel of a sprite.
 */
constexpr float POSITION_TOLERANCE = 0.01F;

/**
 * @brief Where the center of a ship stops against each edge of the play area:
 * half a ship away from it, so the whole ship stays in sight.
 */
constexpr float LEFT_EDGE_STOP_X = SHIP_WIDTH * engine::HALF;
constexpr float RIGHT_EDGE_STOP_X =
    PLAYFIELD_WIDTH - (SHIP_WIDTH * engine::HALF);
constexpr float TOP_EDGE_STOP_Y = SHIP_HEIGHT * engine::HALF;
constexpr float BOTTOM_EDGE_STOP_Y =
    PLAY_AREA_HEIGHT - (SHIP_HEIGHT * engine::HALF);

static_assert(
    SHIP_SPEED *
            std::chrono::duration<float>(UNOBSTRUCTED_MOVE_DURATION).count() <
        PLAY_AREA_CENTER.y - TOP_EDGE_STOP_Y,
    "UNOBSTRUCTED_MOVE_DURATION of movement up or down from "
    "PLAY_AREA_CENTER meets no edge, so a test of a direction sees "
    "the whole distance");
static_assert(SHIP_SPEED < RIGHT_EDGE_STOP_X - PLAY_AREA_CENTER.x,
              "a second of movement to the right from PLAY_AREA_CENTER meets "
              "no edge, so a test of a straight line sees the whole distance");
static_assert(NEAR_AN_EDGE < SHIP_SPEED,
              "a second of movement from NEAR_AN_EDGE takes a ship past the "
              "edge, so a test of an edge sees it stop");

}  // namespace rtype::game::testing
