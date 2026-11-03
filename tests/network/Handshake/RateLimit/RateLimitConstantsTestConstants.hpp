#pragma once

#include <chrono>
#include <cstddef>

namespace rtype::network::testing {

/**
 * @brief What a client sends while it joins, from the protocol page: a
 * request every 500 ms and its answer to each challenge, a round trip later,
 * until it gives up after 5 s.
 */
constexpr std::chrono::milliseconds CLIENT_REQUEST_INTERVAL{500};
constexpr std::chrono::milliseconds CHALLENGE_ROUND_TRIP{1};
constexpr std::chrono::seconds CLIENT_GIVE_UP{5};

/** @brief The answers a client sends in a tick after a stall of the server. */
constexpr std::size_t ANSWERS_AFTER_A_STALL = 10;

/**
 * @brief Ten times the requests a client sends in a second, at once: enough to
 * be banned.
 */
constexpr std::size_t REQUEST_FLOOD_COUNT = 40;

/**
 * @brief What a player sends, from the protocol page: 60 inputs and a
 * heartbeat a second, and a second of inputs at once.
 */
constexpr std::size_t INPUTS_PER_SECOND = 60;
constexpr std::size_t INPUTS_AT_ONCE = 60;

/**
 * @brief How long the tests watch a sender that keeps to the pace of the
 * protocol: ten minutes, long enough for any surplus over a limit to outgrow
 * its burst.
 */
constexpr std::chrono::minutes LONG_RUN_DURATION{10};

/** @brief The ban of the protocol page. */
constexpr std::chrono::seconds PROTOCOL_BAN_DURATION{10};

}  // namespace rtype::network::testing
