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

/** @brief Adds the datagrams of @p part to @p total, in both directions. */
void addTotals(TrafficTotals& total, const TrafficTotals& part);

/**
 * @returns What crossed the socket between @p older and @p newer, two totals of
 * the same counters, @p newer taken after @p older, so it holds at least as
 * much in each direction.
 */
[[nodiscard]] TrafficTotals trafficBetween(const TrafficTotals& older,
                                           const TrafficTotals& newer);

}  // namespace rtype::network
