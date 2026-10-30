#pragma once

#include <cstdint>
#include "Position.hpp"
#include "ShipDirections.hpp"
#include "ShipMotion.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

/** @brief A ship as ShipControl moves it: where its center is, in logical
 * units, and what moves it besides the directions held. */
struct MovingShip {
  Position position{};
  ShipMotion motion{};
};

/** @brief A new input of a player for its ship: the directions it held
 * before, the ones it holds now, and its inputNumber. */
struct HeldChange {
  ShipDirections before{};
  ShipDirections now{};
  std::uint32_t inputNumber{};
};

/**
 * @brief Where @p from is after @p elapsed, its player holding @p held, and
 * how it moves then.
 *
 * Along each axis the velocity drifts towards the direction held times
 * SHIP_SPEED, as fast along a diagonal as along a straight line, the way
 * DRIFT_RISE_RATE and the constants after it tell. A reversal
 * chases the new target from the current speed. A maneuver carries the ship
 * on top, its distance eased out over its duration. The ship stops where its
 * side meets an edge of the play area, the playfield less the status band
 * (PLAY_AREA_HEIGHT), and loses its velocity along that axis; a ship that
 * started outside comes back the least needed.
 *
 * The server runs it for each ship at every tick of a game in progress. The
 * client is meant to run it to predict its own ship, and does not yet.
 *
 * @param elapsed Zero or more.
 */
[[nodiscard]] MovingShip shipAfter(MovingShip from, ShipDirections held,
                                   engine::Duration elapsed);

/**
 * @returns @p motion with the presses of @p change recorded, and a maneuver
 * started by a direction pressed again within DOUBLE_TAP_INPUTS inputs of its
 * last press: down or up rolls the ship ROLL_SIDESTEP that way, right dashes
 * it DASH_DISTANCE forward, left BACKSTEP_DISTANCE back. Nothing starts while
 * a maneuver runs, nor a roll before ROLL_COOLDOWN has passed since the last
 * roll ended, nor a dash or a backstep before DASH_COOLDOWN has passed since
 * the last of them ended. A second press that starts a maneuver is not a
 * first press of the next one.
 */
[[nodiscard]] ShipMotion motionAfterInput(ShipMotion motion, HeldChange change);

}  // namespace rtype::game
