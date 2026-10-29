#pragma once

#include <chrono>
#include <cstdint>
#include <limits>

namespace rtype::server::testing {

/**
 * @brief A next sequence a player reaches after a few datagrams of its
 * session.
 */
constexpr std::uint16_t SOME_NEXT_SEQUENCE = 7;

/**
 * @brief A time the server sent a player nothing for, short of a Heartbeat.
 */
constexpr std::chrono::milliseconds SOME_TIME_SINCE_SENT{700};

/**
 * @brief The last sequence before the counter of a session wraps to 0.
 */
constexpr std::uint16_t LAST_SEQUENCE =
    std::numeric_limits<std::uint16_t>::max();

}  // namespace rtype::server::testing
