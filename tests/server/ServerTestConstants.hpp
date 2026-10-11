#pragma once

#include <chrono>
#include <cstddef>
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
 * @brief How long a test waits for a thread to start or stop; only a failing
 * test waits that long.
 */
constexpr std::chrono::seconds THREAD_TIMEOUT{5};

/**
 * @brief Ticks a test lets a server run before stopping it, so that run() has
 * most likely started.
 */
constexpr int SERVER_RUN_TICKS = 3;
