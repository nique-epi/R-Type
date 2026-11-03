#pragma once

#include <chrono>
#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief How many of the newest samples of an entity are kept: two to
 * interpolate between, two more so that one lost state in a row still leaves
 * a pair around the tick being shown. */
constexpr std::size_t HISTORY_SAMPLES = 4;

/** @brief The longest an entity is moved on beyond its newest sample, from the
 * speed between its last two samples: no state says where it went. */
constexpr engine::Duration MAXIMUM_EXTRAPOLATION =
    std::chrono::milliseconds(100);

/** @brief MAXIMUM_EXTRAPOLATION in server ticks: 6 at 60 ticks per second. */
constexpr double MAXIMUM_EXTRAPOLATION_TICKS =
    std::chrono::duration<double>(MAXIMUM_EXTRAPOLATION).count() *
    static_cast<double>(engine::SIMULATION_TICKS_PER_SECOND);

}  // namespace rtype::client
