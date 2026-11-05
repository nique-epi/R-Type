#pragma once

#include <chrono>
#include <cstdint>

namespace rtype::network::testing {

/**
 * @brief The round trip after which a joining client answers a challenge. A
 * client whose server stalled answers all its challenges a round trip after
 * its last request too, the moment its allowance has the least room left.
 */
constexpr std::chrono::milliseconds CHALLENGE_ROUND_TRIP{1};

/**
 * @brief How long the tests watch a sender that keeps to the pace of the
 * protocol: ten minutes, long enough for any surplus over a limit to outgrow
 * its burst.
 */
constexpr std::chrono::minutes LONG_RUN_DURATION{10};

/**
 * @brief How often each limit gives a datagram back, from the table of limits
 * of the protocol page: four requests a second, and 120 datagrams of a player
 * a second.
 */
constexpr std::chrono::milliseconds PROTOCOL_REQUEST_REFILL{250};
constexpr std::int64_t PROTOCOL_PLAYER_DATAGRAMS_PER_SECOND = 120;
constexpr std::chrono::nanoseconds PROTOCOL_PLAYER_REFILL =
    std::chrono::nanoseconds{std::chrono::seconds{1}} /
    PROTOCOL_PLAYER_DATAGRAMS_PER_SECOND;

/** @brief The ban of the protocol page. */
constexpr std::chrono::seconds PROTOCOL_BAN_DURATION{10};

}  // namespace rtype::network::testing
