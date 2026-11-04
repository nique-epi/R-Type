#pragma once

#include <chrono>

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

/** @brief The ban of the protocol page. */
constexpr std::chrono::seconds PROTOCOL_BAN_DURATION{10};

}  // namespace rtype::network::testing
