#pragma once

#include <chrono>
#include <cstddef>

/**
 * @brief The requests one address and port places at once: the ten answers a
 * client sends in one tick after a stall of the server.
 */
constexpr std::size_t REQUESTS_A_CLIENT_SENDS_AT_ONCE = 10;

/** @brief The datagrams one player places at once: a second of its inputs. */
constexpr std::size_t DATAGRAMS_A_PLAYER_SENDS_AT_ONCE = 60;

/**
 * @brief A flood of a player with its own token that ends in a ban: a second
 * of inputs, then the sixty drops that ban it.
 */
constexpr std::size_t PLAYER_FLOOD_TO_A_BAN = 120;

/**
 * @brief A nanosecond before, and the end of, a ban of the protocol page,
 * 10 s after it started.
 */
constexpr std::chrono::nanoseconds JUST_BEFORE_THE_BAN_ENDS =
    std::chrono::seconds{10} - std::chrono::nanoseconds{1};
constexpr std::chrono::seconds WHEN_THE_BAN_ENDS{10};
