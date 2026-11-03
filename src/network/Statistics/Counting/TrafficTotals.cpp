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

}  // namespace rtype::network
