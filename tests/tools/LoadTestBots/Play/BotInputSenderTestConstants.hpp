#pragma once

#include <cstddef>
#include "TimeConstants.hpp"

namespace rtype::tools::testing {

/** @brief A second of frames of SIMULATION_TICK_DURATION. */
constexpr std::size_t FRAMES_IN_A_SECOND = 60;

/** @brief The inputs a client sends in a second of game, as the tool
 * requires of each bot. */
constexpr std::size_t INPUTS_IN_A_SECOND = 60;

/** @brief A frame twice as short as a tick, and a second of them. */
constexpr engine::Duration HALF_A_TICK = engine::SIMULATION_TICK_DURATION / 2;
constexpr std::size_t HALF_TICKS_IN_A_SECOND = 120;

}  // namespace rtype::tools::testing
