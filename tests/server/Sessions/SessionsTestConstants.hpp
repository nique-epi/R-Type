#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>

namespace rtype::server::testing {

/**
 * @brief The sequence of the datagram a message came in, and the sequences
 * just before and after it.
 */
constexpr std::uint16_t SOME_SEQUENCE = 40;
constexpr std::uint16_t OLDER_SEQUENCE = SOME_SEQUENCE - 1;
constexpr std::uint16_t NEWER_SEQUENCE = SOME_SEQUENCE + 1;

/** @brief The last sequence of a 16-bit counter, and the one that follows it.
 */
constexpr std::uint16_t LAST_SEQUENCE = 65535;
constexpr std::uint16_t FIRST_SEQUENCE = 0;

/**
 * @brief Numbers of the inputs a player sent, in the order it numbers them.
 */
constexpr std::uint32_t SOME_INPUT_NUMBER = 100;
constexpr std::uint32_t OLDER_INPUT_NUMBER = SOME_INPUT_NUMBER - 1;
constexpr std::uint32_t NEWER_INPUT_NUMBER = SOME_INPUT_NUMBER + 1;

/**
 * @brief Two sets of actions that differ.
 */
constexpr std::uint8_t SOME_ACTIONS = 0x01;
constexpr std::uint8_t OTHER_ACTIONS = 0x04;

/**
 * @brief How many players the keepers are given.
 */
constexpr std::size_t TWO_PLAYERS = 2;

/**
 * @brief The 500 ms of the protocol page between two Heartbeats, rounded down
 * to the 30 whole ticks of 16 666 666 ns that fit in it: the server sends at
 * the last tick that does not pass the interval.
 */
constexpr std::chrono::nanoseconds PROTOCOL_TIME_BETWEEN_HEARTBEATS{
    499'999'980};

/**
 * @brief When the server sends a Heartbeat in a test, and that time as the
 * Heartbeat carries it, in microseconds.
 */
constexpr std::chrono::seconds HEARTBEAT_SENDING_TIME{3};
constexpr std::uint32_t HEARTBEAT_SENDING_TIME_ON_THE_WIRE = 3'000'000;

/**
 * @brief A Heartbeat of a player that arrives 100 ms before the server sends
 * its own: the time the player sent it, when it arrives, and how long the
 * server holds it, in microseconds.
 */
constexpr std::uint32_t PLAYER_HEARTBEAT_SENT_AT = 9;
constexpr std::chrono::milliseconds PLAYER_HEARTBEAT_ARRIVAL{2900};
constexpr std::uint32_t HELD_UNTIL_THE_SERVER_SENDS = 100'000;

/**
 * @brief The echo of the Heartbeat the server sent at HEARTBEAT_SENDING_TIME,
 * held 2 ms by the client, which comes back 50 ms after it left: a round trip
 * of 48 ms.
 */
constexpr std::uint32_t HELD_BY_THE_CLIENT = 2'000;
constexpr std::chrono::milliseconds ECHO_ARRIVAL{3050};
constexpr std::chrono::milliseconds MEASURED_PLAYER_ROUND_TRIP{48};
constexpr std::uint32_t MEASURED_PLAYER_ROUND_TRIP_ON_THE_WIRE = 48'000;

/**
 * @brief The time on the clock of the sessions when a datagram of a player
 * arrived, 2 s after the clock started.
 */
constexpr std::chrono::seconds SOME_ARRIVAL_TIME{2};

/**
 * @brief The real time a tick of the server took, about one sixtieth of a
 * second.
 */
constexpr std::chrono::milliseconds SOME_TICK_TIME{17};

/**
 * @brief A stall of the server longer than the 1 s between two Heartbeats:
 * no tick ran for 3 s.
 */
constexpr std::chrono::seconds SOME_STALL{3};

}  // namespace rtype::server::testing
