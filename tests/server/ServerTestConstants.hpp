#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "ServerConstants.hpp"
#include "TimeConstants.hpp"

/**
 * @brief Number of payload bytes the client of the server tests sends.
 */
constexpr std::size_t CLIENT_PAYLOAD_SIZE = 3;

/**
 * @brief The value of every payload byte the client of the server tests
 * sends.
 */
constexpr std::byte CLIENT_PAYLOAD_BYTE{0x2A};

/**
 * @brief Simulation ticks between two world states at the default state rate,
 * which divides the tick rate.
 */
constexpr std::size_t DEFAULT_TICKS_PER_WORLD_STATE =
    rtype::engine::SIMULATION_TICKS_PER_SECOND /
    rtype::server::DEFAULT_WORLD_STATES_PER_SECOND;

static_assert(rtype::engine::SIMULATION_TICKS_PER_SECOND %
                      rtype::server::DEFAULT_WORLD_STATES_PER_SECOND ==
                  0,
              "the tests that count ticks between states need a default rate "
              "that divides the tick rate");

/**
 * @brief Half of one simulation tick: SIMULATION_TICK_DURATION is an even
 * number of nanoseconds, so two halves make exactly one tick.
 */
constexpr rtype::engine::Duration HALF_TICK =
    rtype::engine::SIMULATION_TICK_DURATION / 2;

/**
 * @brief Ticks a running loop must have run before a test stops it.
 */
constexpr int TICKS_BEFORE_STOP = 3;

/**
 * @brief Ticks a loop misses when its clock jumps forward at once.
 */
constexpr int MISSED_TICK_COUNT = 3;

/**
 * @brief Ticks due at once in the lateness tests: the first one late, the
 * second less so.
 */
constexpr int TICKS_DUE_TOGETHER = 2;

/**
 * @brief Length of a stall, in ticks, far longer than the ticks a loop
 * catches up, so its excess is dropped.
 */
constexpr int STALL_TICK_COUNT = 60;

/**
 * @brief Length of a second stall, in ticks, shorter than STALL_TICK_COUNT but
 * still longer than the ticks a loop catches up.
 */
constexpr int SHORTER_STALL_TICK_COUNT = 10;

/**
 * @brief Ticks a loop runs after a stall before it drops the rest:
 * MAXIMUM_TICKS_PER_ADVANCE, as a count to multiply a tick duration by.
 */
constexpr int CAUGHT_UP_TICK_COUNT =
    static_cast<int>(rtype::engine::MAXIMUM_TICKS_PER_ADVANCE);

/**
 * @brief How long a test waits for a thread to start or stop; only a failing
 * test waits that long.
 */
constexpr std::chrono::seconds THREAD_TIMEOUT{5};

/**
 * @brief Ticks a test lets a server run before stopping it, so that run() has
 * most likely started.
 */
constexpr int SERVER_RUN_TICKS = 3;

/**
 * @brief Senders of the sorting tests: an admitted player, a stranger, and a
 * second player for the tests that need two.
 */
constexpr std::string_view SORTED_SENDER_ADDRESS = "192.0.2.40";
constexpr std::uint16_t PLAYER_PORT = 41000;
constexpr std::uint16_t STRANGER_PORT = 41001;
constexpr std::uint16_t SECOND_PLAYER_PORT = 41002;

/**
 * @brief A flood of connection requests: twice what the player queue holds,
 * so that requests sent to the player queue would push a player's datagram
 * out of it.
 */
constexpr std::size_t FLOODING_REQUEST_COUNT =
    2 * rtype::server::INCOMING_DATAGRAM_QUEUE_CAPACITY;
