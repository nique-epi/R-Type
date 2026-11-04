#pragma once

#include <cstddef>
#include <vector>
#include "BoundedQueue.hpp"

/** @returns @p count numbers, from @p first on. */
std::vector<int> numbered(int first, std::size_t count);

/** @brief Pushes each of @p messages into @p queue, in order. */
void pushAll(rtype::concurrency::BoundedQueue<int>& queue,
             const std::vector<int>& messages);

/**
 * @returns What @p queue held, oldest first; the queue is then empty. A test
 * that only empties the queue leaves the result.
 */
std::vector<int> drained(rtype::concurrency::BoundedQueue<int>& queue);
