#pragma once

#include "Position.hpp"

namespace rtype::client::testing {

constexpr game::Position ORIGIN{.x = 0.0F, .y = 0.0F};

/** @brief A ship predicted at (160, 200), and the ships shown 60 units behind
 * it and 460 units behind it, on the same row. */
constexpr game::Position PREDICTED{.x = 160.0F, .y = 200.0F};
constexpr game::Position SHOWN_SIXTY_BEHIND{.x = 100.0F, .y = 200.0F};
constexpr game::Position SHOWN_FAR_BEHIND{.x = -300.0F, .y = 200.0F};

/** @brief What is left of a gap of 60 units after one tick that keeps 90
 * percent of it. */
constexpr float SIXTY_UNITS_AFTER_ONE_TICK = 54.0F;

/** @brief How much larger than the snap distance the gap of the test that
 * must snap is. */
constexpr float ABOVE_THE_SNAP_DISTANCE = 1.0F;

}  // namespace rtype::client::testing
