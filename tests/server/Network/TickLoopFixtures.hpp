#pragma once

#include "SimulatedClock.hpp"
#include "TickLoop.hpp"
#include "TimeConstants.hpp"

/**
 * @brief Lets @p tickCount ticks and a half pass at once, then runs the ticks
 * due.
 */
void stallFor(SimulatedClock& clock, rtype::server::TickLoop& loop,
              int tickCount);

/**
 * @brief The game time a stall of @p tickCount ticks and a half drops: all of
 * it past the ticks caught up.
 */
[[nodiscard]] rtype::engine::Duration droppedBy(int tickCount);
