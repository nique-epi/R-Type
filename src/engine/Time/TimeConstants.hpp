#pragma once

#include <chrono>
#include <cstddef>

namespace rtype::engine {

using Duration = std::chrono::nanoseconds;

constexpr std::size_t SIMULATION_TICKS_PER_SECOND = 60;
constexpr Duration SIMULATION_TICK_DURATION =
    std::chrono::duration_cast<Duration>(std::chrono::seconds(1)) /
    SIMULATION_TICKS_PER_SECOND;
constexpr std::size_t MAXIMUM_TICKS_PER_ADVANCE = 5;

}  // namespace rtype::engine
