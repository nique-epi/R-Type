#pragma once

#include <chrono>
#include <cstddef>
#include "RateLimitConstantsTestConstants.hpp"
#include "TimeTestConstants.hpp"

/**
 * @brief The connection requests one address and port places at once, from
 * the table of limits of the protocol page: eleven.
 */
constexpr std::size_t REQUESTS_PLACED_AT_ONCE = 11;

/**
 * @brief The datagrams one player places at once with its token, from the
 * table of limits of the protocol page: a second of inputs, sixty.
 */
constexpr std::size_t PLAYER_DATAGRAMS_PLACED_AT_ONCE = 60;

/**
 * @brief The drops that ban a player flooding with its own token, from the
 * protocol page: sixty within a second.
 */
constexpr std::size_t DROPS_THAT_BAN_A_PLAYER = 60;

/**
 * @brief A flood of a player with its own token that ends in a ban: a second
 * of inputs, then the drops that ban it.
 */
constexpr std::size_t PLAYER_FLOOD_TO_A_BAN =
    PLAYER_DATAGRAMS_PLACED_AT_ONCE + DROPS_THAT_BAN_A_PLAYER;

/** @brief A nanosecond before the end of a ban of the protocol page. */
constexpr std::chrono::nanoseconds JUST_BEFORE_THE_BAN_ENDS =
    rtype::network::testing::PROTOCOL_BAN_DURATION - ONE_NANOSECOND;
