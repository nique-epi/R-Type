#pragma once

#include <chrono>
#include <cstddef>
#include "ProtocolConstants.hpp"

namespace rtype::network {

/**
 * @brief Each round trip moves the smoothed round trip by this fraction of its
 * gap to it, 1/8, the alpha of RFC 6298 (section 2.3) and of RFC 9002
 * (section 5.3).
 */
constexpr std::chrono::microseconds::rep ROUND_TRIP_SMOOTHING_DIVISOR = 8;

/**
 * @brief How many of the newest round trips the jitter is computed on: 16, so
 * 8 s at one Heartbeat every HEARTBEAT_INTERVAL.
 */
constexpr std::size_t JITTER_WINDOW_SIZE = 16;

/**
 * @brief The longest round trip a RoundTripMeter accepts, and the longest it
 * keeps a Heartbeat to echo: a side takes its peer for gone after
 * SESSION_SILENCE_LIMIT, so a longer one comes from a sentAt this side never
 * sent in the session.
 */
constexpr std::chrono::microseconds LONGEST_ROUND_TRIP = SESSION_SILENCE_LIMIT;

}  // namespace rtype::network
