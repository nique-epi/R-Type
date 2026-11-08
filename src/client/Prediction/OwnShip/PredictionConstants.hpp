#pragma once

#include <chrono>
#include <cstddef>
#include "ShipControlConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief What a ship at full speed covers in one simulation tick, in logical
 * units. The ship of the server can be that far ahead of the position it has
 * after the input it confirms, since it repeats the directions of a tick that
 * brought no new input. */
constexpr float TICK_DISTANCE =
    game::SHIP_SPEED *
    std::chrono::duration<float>(engine::SIMULATION_TICK_DURATION).count();

/** @brief The commands a prediction keeps while the player has no ship, for
 * the next ship to play the ones the server applies to it: two seconds, far
 * more than a round trip and the input delay. */
constexpr std::size_t KEPT_COMMANDS_BEFORE_A_SHIP =
    2 * engine::SIMULATION_TICKS_PER_SECOND;

/** @brief How many ticks of distance the prediction tolerates between the
 * ship it predicted after a command and the position the server confirms for
 * that command: one tick is explained by the repeated directions, the second
 * keeps the margin of a datagram that arrives a tick late. */
constexpr std::size_t RECONCILIATION_TOLERANCE_TICKS = 2;

/** @brief The distance, in logical units, under which the client keeps its own
 * position instead of taking the one of the server back. */
constexpr float RECONCILIATION_TOLERANCE =
    TICK_DISTANCE * static_cast<float>(RECONCILIATION_TOLERANCE_TICKS);

}  // namespace rtype::client
