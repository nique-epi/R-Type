#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "TimeConstants.hpp"

/**
 * @brief Room of the queues the relay tests use.
 */
constexpr std::size_t RELAY_QUEUE_CAPACITY = 4;

/**
 * @brief Address of the peer the relay tests exchange datagrams with.
 */
constexpr std::string_view PEER_ADDRESS = "192.0.2.7";

/**
 * @brief Port of the peer the relay tests exchange datagrams with.
 */
constexpr std::uint16_t PEER_PORT = 4243;

/**
 * @brief Port of a second peer, so two destinations tell apart.
 */
constexpr std::uint16_t OTHER_PEER_PORT = 4244;

/**
 * @brief A byte the receive buffer holds before a test overwrites it.
 */
constexpr std::byte RECEIVED_BYTE{0x2A};

/**
 * @brief The byte a test writes over the receive buffer after the handler
 * returned.
 */
constexpr std::byte OVERWRITTEN_BYTE{0x00};

/**
 * @brief Number of payload bytes the relay tests send and receive.
 */
constexpr std::size_t RELAY_PAYLOAD_SIZE = 3;

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
