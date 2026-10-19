#pragma once

#include <cstddef>
#include "BydoSpawnerConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server::testing {

/**
 * @brief The ticks it takes for a whole BYDO_SPAWN_INTERVAL to pass: the tick
 * lasts a whole number of nanoseconds, so the last of these ticks is the first
 * to reach the interval.
 */
constexpr std::size_t TICKS_PER_BYDO_INTERVAL = static_cast<std::size_t>(
    (BYDO_SPAWN_INTERVAL + engine::SIMULATION_TICK_DURATION -
     engine::Duration{1}) /
    engine::SIMULATION_TICK_DURATION);

}  // namespace rtype::server::testing
