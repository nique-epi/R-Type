#pragma once

#include <cstddef>

/**
 * @brief Room of the queues the single-thread tests fill and overfill.
 */
constexpr std::size_t SMALL_CAPACITY = 3;

/**
 * @brief Room of the smallest queue that can exist.
 */
constexpr std::size_t SINGLE_MESSAGE_CAPACITY = 1;

/**
 * @brief How many messages an overfilled queue receives beyond its room.
 */
constexpr std::size_t EXTRA_MESSAGE_COUNT = 2;

/**
 * @brief Number of the first message pushed after a drain, far from the
 * numbers pushed before it.
 */
constexpr int SECOND_ROUND_FIRST_MESSAGE = 10;

/**
 * @brief Number of messages the writer thread pushes in the two-thread tests:
 * enough for the reader to drain many times while the writer pushes.
 */
constexpr std::size_t CONCURRENT_MESSAGE_COUNT = 100'000;

/**
 * @brief Room of the queue that must overflow while two threads use it.
 */
constexpr std::size_t OVERFLOWING_CAPACITY = 8;

/**
 * @brief A message already in the destination before a drain.
 */
constexpr int EARLIER_MESSAGE = 99;

/**
 * @brief The value a move-only message carries through the queue.
 */
constexpr int CARRIED_VALUE = 7;
