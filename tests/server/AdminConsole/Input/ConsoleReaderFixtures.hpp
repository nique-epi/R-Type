#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include "BoundedQueue.hpp"

/**
 * @returns The lines @p lines received, oldest first, once there are
 * @p count of them or WAIT_FOR_THREADS passed: the thread of the reader hands
 * them whenever it runs.
 */
[[nodiscard]] std::vector<std::string> linesWithin(
    rtype::concurrency::BoundedQueue<std::string>& lines, std::size_t count);
