#include <gtest/gtest.h>
#include "MessageType.hpp"
#include "TrafficCounters.hpp"
#include "TrafficFigures.hpp"
#include "TrafficFixtures.hpp"
#include "TrafficReport.hpp"
#include "TrafficTestConstants.hpp"
#include "TrafficTotals.hpp"

using rtype::network::MessageType;
using rtype::network::TrafficCounters;
using rtype::network::TrafficFigures;
using rtype::network::TrafficReport;
using rtype::network::TrafficTotals;
using rtype::network::trafficTotalsOf;
using rtype::network::testing::datagramHolding;
using rtype::network::testing::loopbackOnPort;

/**
 * Given a report of nothing
 * When its totals are asked
 * Then they are zero
 */
TEST(TrafficTotals, AnEmptyReportTotalsNothing) {
  EXPECT_EQ(trafficTotalsOf(TrafficReport{}), TrafficTotals{});
}

/**
 * Given a datagram in each row: a followed peer, a departed peer, the others
 * When the totals are asked
 * Then they add up the three
 */
TEST(TrafficTotals, AddsUpEveryRow) {
  TrafficCounters counters;
  const auto datagram = datagramHolding(MessageType::Heartbeat, SMALL_PAYLOAD);
  counters.follow(loopbackOnPort(PEER_PORT));
  counters.countSent(loopbackOnPort(PEER_PORT), datagram);
  counters.unfollow(loopbackOnPort(PEER_PORT));
  counters.follow(loopbackOnPort(OTHER_PORT));
  counters.countSent(loopbackOnPort(OTHER_PORT), datagram);
  counters.countSent(loopbackOnPort(PEER_PORT), datagram);

  EXPECT_EQ(trafficTotalsOf(counters.report()).sent,
            (TrafficFigures{
                .count = 3,
                .bytes = 3 * (HEADER_BYTES + PREFIX_BYTES + SMALL_PAYLOAD)}));
}
