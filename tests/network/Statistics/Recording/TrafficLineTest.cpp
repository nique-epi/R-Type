#include <gtest/gtest.h>
#include <chrono>
#include "TrafficFigures.hpp"
#include "TrafficLine.hpp"
#include "TrafficTestConstants.hpp"
#include "TrafficTotals.hpp"

using rtype::network::TrafficFigures;
using rtype::network::trafficLineOf;
using rtype::network::TrafficTotals;

/**
 * Given totals ten seconds apart
 * When the line is written
 * Then it gives what crossed between them, and the bytes per second
 */
TEST(TrafficLine, SaysWhatCrossedBetweenTwoTotals) {
  const TrafficTotals older{
      .sent = TrafficFigures{.count = 10, .bytes = 1000},
      .received = TrafficFigures{.count = 1, .bytes = 10}};
  const TrafficTotals newer{
      .sent = TrafficFigures{.count = 30, .bytes = 3000},
      .received = TrafficFigures{.count = 6, .bytes = 60}};

  EXPECT_EQ(trafficLineOf(older, newer, TEN_SECONDS),
            "traffic over 10 s: sent 20 datagrams, 2000 bytes (200 B/s); "
            "received 5 datagrams, 50 bytes (5 B/s)");
}

/**
 * Given totals with no time between them
 * When the line is written
 * Then the rate is zero, not a division by zero
 */
TEST(TrafficLine, NoTimeGivesNoRate) {
  const TrafficTotals newer{.sent = TrafficFigures{.count = 1, .bytes = 100},
                            .received = TrafficFigures{}};

  EXPECT_EQ(trafficLineOf(TrafficTotals{}, newer, std::chrono::nanoseconds{0}),
            "traffic over 0 s: sent 1 datagrams, 100 bytes (0 B/s); "
            "received 0 datagrams, 0 bytes (0 B/s)");
}
