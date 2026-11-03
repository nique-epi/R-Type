#pragma once

#include <chrono>
#include <string>
#include "TrafficReport.hpp"

namespace rtype::network {

/**
 * @returns The lines of a traffic file for @p report, taken @p elapsed after
 * the start: `seconds,peer,direction,message,count,bytes`, one per figure
 * that is not zero. The message `all` gives the whole datagrams, `unreadable`
 * those whose messages are not counted. Nothing when nothing was counted.
 */
[[nodiscard]] std::string trafficCsvRowsOf(std::chrono::nanoseconds elapsed,
                                           const TrafficReport& report);

}  // namespace rtype::network
