#pragma once

#include <cstddef>
#include "Position.hpp"

namespace rtype::server::testing {

/** @brief The ticks left of the transition the hold tests play. */
constexpr std::size_t TICKS_LEFT_OF_THE_TRANSITION = 120;

/** @brief How fast a ship drifts to the right before it is held, in logical
 * units per second. */
constexpr float DRIFTING_SPEED = 300.0F;

/** @brief Where a ship flew before the ships come back, far from any start. */
constexpr game::Position WHERE_THE_SHIP_FLEW{.x = 1500.0F, .y = 900.0F};

}  // namespace rtype::server::testing
