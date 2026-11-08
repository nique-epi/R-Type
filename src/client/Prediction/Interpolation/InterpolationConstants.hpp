#pragma once

#include <cstddef>

namespace rtype::client {

/** @brief How many state intervals behind the newest state the other entities
 * are shown. With 2, a state lost in a row still leaves a sample on each side
 * of the tick shown; two lost in a row are covered by extrapolation. */
constexpr double INTERPOLATION_INTERVALS = 2.0;

/** @brief The interval between two states assumed until two have arrived:
 * 3 ticks, 50 ms, the default rate of 20 states per second. */
constexpr double ASSUMED_STATE_INTERVAL_TICKS = 3.0;

/** @brief How many of the newest gaps between two states received are kept to
 * find the interval: the smallest one, since a lost state only widens a gap. */
constexpr std::size_t INTERVAL_WINDOW = 8;

/** @brief How many of the newest states received keep their lateness: the
 * largest one sets how far behind the clock of the server the tick shown is,
 * so a late state is remembered for 32 states, 1.6 s at 20 states per second.
 */
constexpr std::size_t LATENESS_WINDOW = 32;

/** @brief A state later than this many ticks, 2 s, was not sent by the server
 * at the tick it carries as far as the clock of the client can tell: the clock
 * is stale, and the state does not count toward the lateness. */
constexpr double LONGEST_PLAUSIBLE_LATENESS_TICKS = 120.0;

/** @brief A state earlier than this many ticks, 100 ms, before the tick the
 * clock of the client puts the server at comes from a clock that is wrong: it
 * does not count toward the lateness. A lateness below 0 inside that margin
 * counts as 0. */
constexpr double EARLIEST_PLAUSIBLE_LATENESS_TICKS = -6.0;

/** @brief The share of the error between the tick shown and the tick it should
 * be at that is taken back each time a state arrives. */
constexpr double CORRECTION_GAIN = 0.1;

/** @brief An error of more than this many intervals is taken back at once
 * instead of gradually: the frame loop stalled or states stopped for a while.
 */
constexpr double SNAP_INTERVALS = 4.0;

}  // namespace rtype::client
