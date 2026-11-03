#pragma once

#include "TrafficFigures.hpp"
#include "TrafficReport.hpp"

namespace rtype::network {

/** @brief The datagrams of every row of a TrafficReport, added up. */
struct TrafficTotals {
  TrafficFigures sent;
  TrafficFigures received;

  bool operator==(const TrafficTotals&) const = default;
};

/**
 * @returns The datagrams of every row of @p report. They only grow, whoever
 * comes and goes: a peer that leaves moves from one row to another.
 */
[[nodiscard]] TrafficTotals trafficTotalsOf(const TrafficReport& report);

}  // namespace rtype::network
