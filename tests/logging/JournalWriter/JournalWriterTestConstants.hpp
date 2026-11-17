#pragma once

#include <chrono>
#include <cstddef>
#include <string_view>

/** @brief The name of the logger of the lines the tests queue. */
constexpr std::string_view WRITER_TEST_LOGGER_NAME = "Test";

/** @brief The lines queued past a full queue: 1808 more than its 8192 places,
 * and the line the thread of the journal holds at the gate. */
constexpr std::size_t LINES_PAST_THE_QUEUE = 1808;

/** @brief How long a test waits for every queue() call to return while the
 * thread of the journal is stuck: far longer than 10 000 calls take, far
 * shorter than the timeout of ctest. */
constexpr std::chrono::seconds QUEUE_CALLS_DEADLINE{10};

/** @brief How long the thread of the journal stays stuck after a line was
 * queued, longer than the clock can blur. */
constexpr std::chrono::milliseconds STUCK_DESTINATION_DELAY{200};

/** @brief How many lines a test queues before it flushes or closes. */
constexpr std::size_t FEW_LINES = 3;

/** @brief The threads that queue lines at once, and the lines each queues. */
constexpr std::size_t QUEUING_THREADS = 4;
constexpr std::size_t LINES_PER_THREAD = 500;
