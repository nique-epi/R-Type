#pragma once

#include <array>
#include <chrono>
#include <cstddef>

/**
 * @brief Bytes the tests exchange with the network thread, each one different
 * from the next, so that a lost, shifted or swapped byte shows.
 */
constexpr std::array<std::byte, 4> EXCHANGED_BYTES{
    std::byte{0x10}, std::byte{0x20}, std::byte{0x30}, std::byte{0x40}};

/**
 * @brief Time a test waits between two drains of the network thread, the way
 * the frame loop drains it once per frame.
 */
constexpr std::chrono::milliseconds DRAIN_INTERVAL{1};

/**
 * @brief Network threads a test creates and destroys at once: enough for some
 * of them to be destroyed before their thread ran the network.
 */
constexpr int IMMEDIATE_DESTRUCTION_COUNT = 100;

/**
 * @brief Network threads a test hands one datagram each, then destroys at
 * once, so the datagram is still waiting when the destructor of most of them
 * runs.
 */
constexpr std::size_t SENDING_THREADS_DESTROYED_AT_ONCE = 32;
