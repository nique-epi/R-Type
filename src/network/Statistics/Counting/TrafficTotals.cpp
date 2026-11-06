#include "TrafficTotals.hpp"
#include "PeerTraffic.hpp"
#include "TrafficFigures.hpp"
#include "TrafficReport.hpp"
#include "TwoWayTraffic.hpp"

namespace rtype::network {

namespace {

void addRow(TrafficTotals& totals, const TwoWayTraffic& row) {
  addFigures(totals.sent, row.sent.datagrams);
  addFigures(totals.received, row.received.datagrams);
}

TrafficFigures figuresBetween(const TrafficFigures& older,
                              const TrafficFigures& newer) {
  return TrafficFigures{.count = newer.count - older.count,
                        .bytes = newer.bytes - older.bytes};
}

}  // namespace

TrafficTotals trafficTotalsOf(const TrafficReport& report) {
  TrafficTotals totals;
  addRow(totals, report.others);
  addRow(totals, report.departed);
  for (const PeerTraffic& peer : report.peers) {
    addRow(totals, peer.traffic);
  }
  return totals;
}

void addTotals(TrafficTotals& total, const TrafficTotals& part) {
  addFigures(total.sent, part.sent);
  addFigures(total.received, part.received);
}

TrafficTotals trafficBetween(const TrafficTotals& older,
                             const TrafficTotals& newer) {
  return TrafficTotals{
      .sent = figuresBetween(older.sent, newer.sent),
      .received = figuresBetween(older.received, newer.received)};
}

}  // namespace rtype::network
