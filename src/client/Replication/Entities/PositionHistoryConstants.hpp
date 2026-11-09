#pragma once

#include <chrono>
#include <cstdint>
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief How many ticks of samples an entity keeps behind its newest one:
 * 3 s, beyond the farthest the clock shows an entity behind its newest state,
 * the worst lateness it accepts (2 s) plus two intervals at the slowest rate
 * of the server, 2 states per second (1 s). Fewer, and a connection whose
 * worst lateness runs far above its usual one shows a tick older than every
 * sample kept: the entity then stands on its oldest sample and moves by whole
 * states. The ticks of the samples only grow, so it keeps at most this many
 * samples plus one. */
constexpr std::uint32_t HISTORY_SPAN_TICKS = 180;

/** @brief The longest an entity is moved on beyond its newest sample, from the
 * speed between its last two samples: no state says where it went. */
constexpr engine::Duration MAXIMUM_EXTRAPOLATION =
    std::chrono::milliseconds(100);

/** @brief MAXIMUM_EXTRAPOLATION in server ticks: 6 at 60 ticks per second. */
constexpr double MAXIMUM_EXTRAPOLATION_TICKS =
    std::chrono::duration<double>(MAXIMUM_EXTRAPOLATION).count() *
    static_cast<double>(engine::SIMULATION_TICKS_PER_SECOND);

}  // namespace rtype::client
