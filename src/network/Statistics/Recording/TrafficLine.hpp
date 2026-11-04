#pragma once

#include <chrono>
#include <string>
#include "TrafficTotals.hpp"

namespace rtype::network {

/**
 * @returns The line of the journal that says what crossed the socket between
 * @p older and @p newer, @p elapsed apart: how many datagrams and bytes each
 * way, and the bytes per second, 0 when no time passed.
 */
[[nodiscard]] std::string trafficLineOf(const TrafficTotals& older,
                                        const TrafficTotals& newer,
                                        std::chrono::nanoseconds elapsed);

}  // namespace rtype::network
