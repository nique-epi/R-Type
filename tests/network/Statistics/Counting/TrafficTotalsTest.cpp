#include <gtest/gtest.h>
#include "MessageType.hpp"
#include "TrafficCounters.hpp"
#include "TrafficFigures.hpp"
#include "TrafficFixtures.hpp"
#include "TrafficReport.hpp"
#include "TrafficTestConstants.hpp"
#include "TrafficTotals.hpp"
#include "TrafficTotalsTestConstants.hpp"

using rtype::network::addTotals;
using rtype::network::MessageType;
using rtype::network::trafficBetween;
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

/**
 * Given the totals of a socket, then its totals once more datagrams crossed it
 * When the traffic between them is asked
 * Then it is what crossed in between, in each direction
 */
TEST(TrafficTotals, TheTrafficBetweenTwoTotalsIsWhatCrossedInBetween) {
  EXPECT_EQ(trafficBetween(EARLIER_TOTALS, LATER_TOTALS), TRAFFIC_IN_BETWEEN);
}

/**
 * Given the totals of a socket
 * When what crossed it next is added to them
 * Then each direction holds both: the later totals
 */
TEST(TrafficTotals, AddingTotalsAddsEachDirection) {
  TrafficTotals total = EARLIER_TOTALS;

  addTotals(total, TRAFFIC_IN_BETWEEN);

  EXPECT_EQ(total, LATER_TOTALS);
}
