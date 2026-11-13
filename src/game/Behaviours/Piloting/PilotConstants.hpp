#pragma once

#include "TimeConstants.hpp"

namespace rtype::game {

/** @brief How long a body rests after an effect of its sheet before it may
 * play another: 15 ticks, a quarter of a second, so one body announces four
 * effects a second at most, whatever its behaviour asks for. */
constexpr engine::Duration EFFECT_REST = 15 * engine::SIMULATION_TICK_DURATION;

}  // namespace rtype::game
