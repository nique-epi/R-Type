#pragma once

#include <chrono>

/**
 * @brief The time on the clock of the sorter when a datagram arrives, 5 s
 * after the clock started.
 */
constexpr std::chrono::seconds SORTED_ARRIVAL_TIME{5};
