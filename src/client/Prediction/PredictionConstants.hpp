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

/** @brief How many ticks of distance the prediction tolerates between the
 * ship it predicted after a command and the position the server confirms for
 * that command: one tick is explained by the repeated directions, the second
 * keeps the margin of a datagram that arrives a tick late. */
constexpr std::size_t RECONCILIATION_TOLERANCE_TICKS = 2;

/** @brief The distance, in logical units, under which the client keeps its own
 * position instead of taking the one of the server back. */
constexpr float RECONCILIATION_TOLERANCE =
    TICK_DISTANCE * static_cast<float>(RECONCILIATION_TOLERANCE_TICKS);

/** @brief How many ticks of distance at full speed a correction may cover
 * before the client stops easing and puts the ship where the server says. */
constexpr float TICKS_BEFORE_SNAP = 15.0F;

/** @brief The distance, in logical units, beyond which the client puts its
 * ship at the position of the server at once instead of easing it there:
 * fifteen ticks at full speed, a quarter of a second. A larger gap is a
 * teleport or a long stall, which a glide across the screen would only hide
 * badly. */
constexpr float SNAP_DISTANCE = TICK_DISTANCE * TICKS_BEFORE_SNAP;

/** @brief The share of the gap between the ship shown and the ship predicted
 * that is still there after one simulation tick: the gap decays
 * exponentially, fast at first and slower as it closes, so the ship does not
 * start or stop with a jerk. The gap halves in about 6.6 ticks (110 ms). */
constexpr float CORRECTION_KEPT_PER_TICK = 0.9F;

/** @brief The gap, in logical units, under which the ship shown is put on the
 * ship predicted: it is too small to see and would take many more ticks to
 * close. */
constexpr float CORRECTION_REST = 0.5F;

}  // namespace rtype::client
