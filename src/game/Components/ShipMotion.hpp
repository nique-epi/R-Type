#pragma once

#include <cstdint>
#include "ManeuverKind.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @brief Component of a player's ship: what moves it besides the directions
 * held this tick. Its velocity, in logical units per second along each axis,
 * drifts towards the directions held (shipAfter()); a maneuver, a roll or a
 * dash, carries it on top for a moment (motionAfterInput()). A ship spawns at
 * rest: a new world or a respawn creates a new ship, so it starts with none.
 */
struct ShipMotion {
  float velocityX{};
  float velocityY{};
  ManeuverKind maneuver{ManeuverKind::None};
  /** @brief How far the whole maneuver carries the ship, in logical units. */
  Position maneuverDistance{};
  /** @brief How long the maneuver has run. */
  engine::Duration maneuverTime{};
  /** @brief How long the ship still waits before it may roll again. */
  engine::Duration rollWait{};
  /** @brief The inputNumber of the last press of each direction, 0 for none:
   * a second press within DOUBLE_TAP_INPUTS of it starts a maneuver. */
  std::uint32_t lastUpPress{};
  std::uint32_t lastDownPress{};
  std::uint32_t lastLeftPress{};
  std::uint32_t lastRightPress{};
};

}  // namespace rtype::game
