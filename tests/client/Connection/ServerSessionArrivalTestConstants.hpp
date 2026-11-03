#pragma once

#include <chrono>
#include <cstdint>

namespace rtype::client::testing {

/**
 * @brief The 500 ms after which the first Heartbeat of a session leaves, on a
 * clock started at 0 when the session opened.
 */
constexpr std::chrono::milliseconds FIRST_HEARTBEAT_TIME{500};

/**
 * @brief The echo of that first Heartbeat: the server held it 2 ms, and the
 * network thread stamped its datagram 50 ms after the Heartbeat left, so the
 * round trip is 48 ms.
 */
constexpr std::uint32_t ECHO_HELD_BY_THE_SERVER = 2'000;
constexpr std::chrono::milliseconds ECHO_STAMPED_AT{550};
constexpr std::chrono::milliseconds ROUND_TRIP_OF_THE_ECHO{48};

}  // namespace rtype::client::testing
