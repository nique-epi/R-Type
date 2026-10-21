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

/**
 * @returns How many whole simulation ticks fit in @p duration, rounded down,
 * so that many ticks of SIMULATION_TICK_DURATION never last longer than
 * @p duration: at 60 ticks per second, 500 ms are 30 ticks, 1 s is 60 and 5 s
 * are 300.
 */
constexpr std::size_t wholeTicksIn(Duration duration) {
  return static_cast<std::size_t>(duration / SIMULATION_TICK_DURATION);
}

}  // namespace rtype::engine
