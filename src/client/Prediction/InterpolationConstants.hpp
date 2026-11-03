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

/** @brief The share of the error between the tick shown and the tick it should
 * be at that is taken back each time a state arrives. */
constexpr double CORRECTION_GAIN = 0.1;

/** @brief An error of more than this many intervals is taken back at once
 * instead of gradually: the frame loop stalled or states stopped for a while.
 */
constexpr double SNAP_INTERVALS = 4.0;

}  // namespace rtype::client
