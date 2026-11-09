#pragma once

#include <cstdint>

namespace rtype::client::testing {

/** @brief The newest state received, a tick drawn a third of a tick before
 * it, and one a third of a tick after it. */
constexpr std::uint32_t NEWEST_STATE_TICK = 300;
constexpr double DRAWN_BEFORE_THE_NEWEST = 299.66;
constexpr double DRAWN_PAST_THE_NEWEST = 300.33;

/** @brief The tick the server is at while the frames draw: 8.34 ticks ahead
 * of the frame drawn before the newest state. */
constexpr double SERVER_TICK_NOW = 308.0;
constexpr double DELAY_BEFORE_THE_NEWEST = 8.34;
constexpr double DELAY_TOLERANCE = 1e-9;

/** @brief The first state of the next game, whose ticks start again. */
constexpr std::uint32_t FIRST_TICK_OF_THE_NEXT_GAME = 3;
constexpr double DRAWN_IN_THE_NEXT_GAME = 3.5;

/** @brief Two moves of the tick shown, and their size added up. */
constexpr double FIRST_MOVE_TICKS = 0.25;
constexpr double SECOND_MOVE_TICKS = 0.5;
constexpr double BOTH_MOVES_TICKS = 0.75;

}  // namespace rtype::client::testing
