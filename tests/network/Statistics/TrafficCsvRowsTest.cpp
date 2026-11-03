#include <gtest/gtest.h>
#include <cstddef>
#include <span>
#include "MessageType.hpp"
#include "TrafficCounters.hpp"
#include "TrafficCsvRows.hpp"
#include "TrafficFixtures.hpp"
#include "TrafficReport.hpp"
#include "TrafficTestConstants.hpp"

using rtype::network::MessageType;
using rtype::network::TrafficCounters;
using rtype::network::trafficCsvRowsOf;
using rtype::network::TrafficReport;
using rtype::network::testing::datagramHolding;
using rtype::network::testing::peerOnPort;

/**
 * Given a report of nothing
 * When its rows are asked
 * Then there are none
 */
TEST(TrafficCsvRows, NothingCountedGivesNoRow) {
  EXPECT_EQ(trafficCsvRowsOf(ONE_SECOND, TrafficReport{}), "");
}

/**
 * Given a followed peer that was sent one message
 * When the rows are asked
 * Then one gives the whole datagram, one the message, and no figure at zero has
 * a row
 */
TEST(TrafficCsvRows, GivesARowForEachFigureOfAPeer) {
  TrafficCounters counters;
  counters.follow(peerOnPort(PEER_PORT));
  counters.countSent(peerOnPort(PEER_PORT),
                     datagramHolding(MessageType::WorldState, LARGE_PAYLOAD));

  EXPECT_EQ(trafficCsvRowsOf(ONE_AND_A_HALF_SECONDS, counters.report()),
            "1.500,127.0.0.1:5000,sent,all,1,125\n"
            "1.500,127.0.0.1:5000,sent,WorldState,1,103\n");
}

/**
 * Given a stranger that sent five bytes nobody can read
 * When the rows are asked
 * Then they are under the others, and say the datagram is unreadable
 */
TEST(TrafficCsvRows, AnUnreadableDatagramOfAStrangerHasItsRow) {
  TrafficCounters counters;
  const auto datagram = datagramHolding(MessageType::Heartbeat, SMALL_PAYLOAD);

  counters.countReceived(
      peerOnPort(PEER_PORT),
      std::span<const std::byte>{datagram}.first(FIVE_BYTES));

  EXPECT_EQ(trafficCsvRowsOf(ONE_SECOND, counters.report()),
            "1.000,others,received,all,1,5\n"
            "1.000,others,received,unreadable,1,5\n");
}

/**
 * Given a datagram each way with a peer that left, and one sent to a stranger
 * and one to a followed peer
 * When the rows are asked
 * Then the peers come first, then the others, then the departed, sent before
 * received
 */
TEST(TrafficCsvRows, ListsThePeersThenTheOthersThenTheDeparted) {
  TrafficCounters counters;
  const auto datagram = datagramHolding(MessageType::Heartbeat, EMPTY_PAYLOAD);
  counters.follow(peerOnPort(PEER_PORT));
  counters.countSent(peerOnPort(PEER_PORT), datagram);
  counters.countReceived(peerOnPort(PEER_PORT), datagram);
  counters.unfollow(peerOnPort(PEER_PORT));
  counters.countSent(peerOnPort(PEER_PORT), datagram);
  counters.follow(peerOnPort(OTHER_PORT));
  counters.countSent(peerOnPort(OTHER_PORT), datagram);

  EXPECT_EQ(trafficCsvRowsOf(ONE_SECOND, counters.report()),
            "1.000,127.0.0.1:5001,sent,all,1,25\n"
            "1.000,127.0.0.1:5001,sent,Heartbeat,1,3\n"
            "1.000,others,sent,all,1,25\n"
            "1.000,others,sent,Heartbeat,1,3\n"
            "1.000,departed,sent,all,1,25\n"
            "1.000,departed,sent,Heartbeat,1,3\n"
            "1.000,departed,received,all,1,25\n"
            "1.000,departed,received,Heartbeat,1,3\n");
}
