#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/**
 * @brief The player the server accepted in the round trip tests, another
 * player of the game, and a player the server never reported.
 */
constexpr std::uint8_t OWN_PLAYER_NUMBER = 1;
constexpr std::uint8_t REPORTED_PLAYER_NUMBER = 2;
constexpr std::uint8_t UNREPORTED_PLAYER_NUMBER = 3;
constexpr std::string_view OWN_NICKNAME = "ACE";

/**
 * @brief The 500 ms of the protocol page between two Heartbeats.
 */
constexpr std::chrono::milliseconds ONE_HEARTBEAT_INTERVAL{500};

/**
 * @brief The time on the clock before the session opens, so the first
 * Heartbeat, one interval later, leaves at 4 s: 4000000 µs on the wire.
 */
constexpr std::chrono::milliseconds CLOCK_WHEN_THE_SESSION_OPENS{3500};
constexpr std::uint32_t FIRST_HEARTBEAT_SENT_AT = 4'000'000;

/**
 * @brief The sequence of the acceptance of the server, then of a datagram of
 * the server that holds guaranteed messages, accepted after it.
 */
constexpr std::uint16_t SERVER_ACCEPTANCE_SEQUENCE = 0;
constexpr std::uint16_t SERVER_GUARANTEED_SEQUENCE = 1;

/**
 * @brief A frame of 1 ms, far short of the interval, and the time a Heartbeat
 * sent at its end carries: 3.501 s, in microseconds.
 */
constexpr std::chrono::milliseconds ONE_FRAME{1};
constexpr std::uint32_t SENT_AT_A_FRAME_AFTER_THE_OPENING = 3'501'000;

/**
 * @brief A Heartbeat of the server sent at 9 µs on its clock, which arrives
 * 10 ms before the first Heartbeat of the client leaves, so it is held
 * 10000 µs.
 */
constexpr std::uint32_t SERVER_HEARTBEAT_SENT_AT = 9;
constexpr std::chrono::milliseconds SERVER_HEARTBEAT_ARRIVAL{3990};
constexpr std::uint32_t HELD_UNTIL_THE_CLIENT_SENDS = 10'000;

/**
 * @brief The echo of the first Heartbeat of the client, held 2 ms by the
 * server, which arrives 50 ms after it left: a round trip of 48 ms.
 */
constexpr std::uint32_t HELD_BY_THE_SERVER = 2'000;
constexpr std::chrono::milliseconds ECHO_ARRIVAL{4050};
constexpr std::chrono::milliseconds OWN_ROUND_TRIP{48};

/** @brief The tick a Heartbeat of the server in a game carries. */
constexpr std::uint32_t SERVER_HEARTBEAT_TICK = 600;
/** @brief Where the clock places that tick: half of the round trip of 48 ms,
 * 24 ms, before the datagram arrived at 4.05 s. */
constexpr std::chrono::milliseconds SERVER_HEARTBEAT_TICK_PLACED_AT{4026};
/** @brief How far the clock must move, from 4 s, to reach the instant the
 * clock placed tick 600 at. */
constexpr std::chrono::milliseconds TO_REACH_THE_PLACED_TICK{26};
/** @brief The tick the server is at on that instant. */
constexpr double TICK_AT_THE_PLACED_INSTANT = 600.0;
/** @brief The room the tick of the server is read in. */
constexpr double SERVER_TICK_TOLERANCE = 1e-6;

/**
 * @brief The round trip and the jitter the server reports for the other
 * player, in microseconds as RoundTrips carries them, and as durations.
 */
constexpr std::uint32_t REPORTED_ROUND_TRIP = 80'000;
constexpr std::uint32_t REPORTED_JITTER = 12'500;
constexpr std::chrono::milliseconds REPORTED_ROUND_TRIP_DURATION{80};
constexpr std::chrono::microseconds REPORTED_JITTER_DURATION{12'500};

}  // namespace rtype::client::testing
