#pragma once

#include <chrono>
#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @brief How far a ship moves in one second, in logical units, along a straight
 * line or a diagonal alike.
 *
 * Provisional, until the game is tuned: at 480 units per second a ship crosses
 * the width of the playfield in 4 seconds and its height in 2.25 seconds, two
 * pixels of a sprite per tick at 60 ticks per second.
 */
constexpr float SHIP_SPEED = 480.0F;

/**
 * @brief Size of a ship, in logical units: the size the client draws it at, 32
 * by 16 pixels of the entity sheet, each covering 4 logical units. A ship stays
 * whole inside the play area, its center half this size away from every edge.
 */
constexpr float SHIP_WIDTH = 128.0F;
constexpr float SHIP_HEIGHT = 64.0F;

/**
 * @brief The drift of a ship: each tick, the speed along an axis closes the
 * share 1 - e^(-rate x tick) of its gap to its target, the direction held
 * times SHIP_SPEED, and never less than floor x tick. The floor ends the
 * curve instead of letting it creep. While a direction of the axis is held,
 * DRIFT_RISE_RATE per second with DRIFT_RISE_FLOOR units per second squared:
 * a ship at rest reaches SHIP_SPEED on its 21st tick.
 */
constexpr float DRIFT_RISE_RATE = 6.0F;
constexpr float DRIFT_RISE_FLOOR = 960.0F;

/**
 * @brief The drift once no direction of the axis is held: a ship let go at
 * SHIP_SPEED glides 85 units and stops on its 28th tick.
 */
constexpr float DRIFT_FALL_RATE = 4.5F;
constexpr float DRIFT_FALL_FLOOR = 720.0F;

/**
 * @brief A barrel roll slides the ship ROLL_SIDESTEP units up or down over
 * ROLL_DURATION, eased out; the next roll waits ROLL_COOLDOWN after it ends.
 */
constexpr float ROLL_SIDESTEP = 112.0F;
constexpr engine::Duration ROLL_DURATION = std::chrono::milliseconds(400);
constexpr engine::Duration ROLL_COOLDOWN = std::chrono::milliseconds(500);

/**
 * @brief A dash carries the ship DASH_DISTANCE units forward, a backstep
 * BACKSTEP_DISTANCE back, over DASH_DURATION, eased out; the next dash or
 * backstep waits DASH_COOLDOWN after it ends, so dashes are not chained back
 * to back.
 */
constexpr float DASH_DISTANCE = 144.0F;
constexpr float BACKSTEP_DISTANCE = 96.0F;
constexpr engine::Duration DASH_DURATION = std::chrono::milliseconds(200);
constexpr engine::Duration DASH_COOLDOWN = std::chrono::milliseconds(500);

/**
 * @brief Inputs a second press of a direction may follow the first by and
 * still start a maneuver. The client sends 60 inputs per second, so 13 is
 * 217 ms.
 */
constexpr std::uint32_t DOUBLE_TAP_INPUTS = 13;

}  // namespace rtype::game
