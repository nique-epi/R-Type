#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include "CollisionConstants.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "ShipControl.hpp"
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

/** @brief A ship at rest in the middle of the play area. */
constexpr MovingShip SHIP_AT_REST{.position = PLAY_AREA_CENTER, .motion = {}};

/** @brief The tick held on which a ship at rest reaches SHIP_SPEED: 350 ms,
 * the full speed of the design. */
constexpr std::size_t FULL_SPEED_TICK = 21;

/** @brief The tick after it is let go on which a ship at SHIP_SPEED stops,
 * and how far it glides: 467 ms and 21.2 pixels of the design, 4 units each. */
constexpr std::size_t STOP_TICK = 28;
constexpr float GLIDE_DISTANCE = 84.94F;

/** @brief Ticks after which a ship holding a direction is surely at full speed.
 */
constexpr std::size_t PAST_FULL_SPEED_TICKS = 60;

/** @brief How far a roll carries a ship: 28 pixels of the design. */
constexpr float ROLL_DISTANCE = 112.0F;

/** @brief How far a dash carries a ship forward, a backstep back: 36 and 24
 * pixels of the design. */
constexpr float DASH_FORWARD_DISTANCE = 144.0F;
constexpr float BACKSTEP_BACK_DISTANCE = 96.0F;

/** @brief The tick on which a roll has carried a ship its whole distance:
 * 400 ms is 24 ticks of 1/60 s, which nanoseconds leave 16 ns short, so the
 * 25th ends it. */
constexpr std::size_t ROLL_END_TICK = 25;

/** @brief Ticks from the end of a roll during which the next may not start:
 * the 500 ms of the design. */
constexpr std::size_t COOLDOWN_TICKS = 30;

/** @brief The tick on which a dash has carried a ship its whole distance:
 * 200 ms is 12 ticks of 1/60 s, which nanoseconds leave 8 ns short, so the
 * 13th ends it. */
constexpr std::size_t DASH_END_TICK = 13;

/** @brief A press of the same direction 13 inputs after QUICK_SECOND_PRESS:
 * within the window of the press that started a dash, once the dash ended. */
constexpr std::uint32_t PRESS_AFTER_THE_DASH = 16;

/** @brief Input numbers of a double press: the first press, its release, a
 * second press within the 220 ms of the design (13 inputs at 60 per second),
 * and one 14 inputs after the first, past it. */
constexpr std::uint32_t FIRST_PRESS = 1;
constexpr std::uint32_t RELEASE = 2;
constexpr std::uint32_t QUICK_SECOND_PRESS = 3;
constexpr std::uint32_t LATE_SECOND_PRESS = 15;

/** @brief How far a ship at rest drifts while its player holds a direction for
 * SIMULATION_TICKS_PER_SECOND ticks, and for its first tick: replayed from the
 * drift of the design (rates 6 and 4.5, floors of 240 and 180 pixels per
 * second squared, 4 units a pixel) outside the C++. */
constexpr float HELD_SECOND_DISTANCE = 417.29F;
constexpr float FIRST_TICK_DISTANCE = 0.7613F;

}  // namespace rtype::game::testing
