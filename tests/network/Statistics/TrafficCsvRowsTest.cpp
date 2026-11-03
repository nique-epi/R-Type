#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <span>
#include <string>
#include "MessageType.hpp"
#include "StatisticsConstants.hpp"
#include "TimeTestConstants.hpp"
#include "TrafficCounters.hpp"
#include "TrafficCsvRows.hpp"
#include "TrafficFixtures.hpp"
#include "TrafficReport.hpp"
#include "TrafficTestConstants.hpp"

using rtype::engine::testing::ONE_SECOND;
using rtype::network::MESSAGE_SLOT_COUNT;
using rtype::network::MessageType;
using rtype::network::TrafficCounters;
using rtype::network::trafficCsvRowsOf;
using rtype::network::TrafficReport;
using rtype::network::testing::datagramHolding;
using rtype::network::testing::datagramOfEveryTypeByte;
using rtype::network::testing::datagramOfTypeByte;
using rtype::network::testing::loopbackOnPort;

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
  counters.follow(loopbackOnPort(PEER_PORT));
  counters.countSent(loopbackOnPort(PEER_PORT),
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
      loopbackOnPort(PEER_PORT),
      std::span<const std::byte>{datagram}.first(FIVE_BYTES));

  EXPECT_EQ(trafficCsvRowsOf(ONE_SECOND, counters.report()),
            "1.000,others,received,all,1,5\n"
            "1.000,others,received,unreadable,1,5\n");
}

/**
 * Given a message of a type byte no message has
 * When the rows are asked
 * Then it has a row called unknown
 */
TEST(TrafficCsvRows, AMessageOfAnUnknownTypeHasARowCalledUnknown) {
  TrafficCounters counters;

  counters.countReceived(
      loopbackOnPort(PEER_PORT),
      datagramOfTypeByte(TYPE_NO_MESSAGE_HAS, SMALL_PAYLOAD));

  EXPECT_EQ(trafficCsvRowsOf(ONE_SECOND, counters.report()),
            "1.000,others,received,all,1,35\n"
            "1.000,others,received,unknown,1,13\n");
}

/**
 * Given one datagram holding a message of every value of a type byte
 * When the rows are asked
 * Then there is one row per slot and one for the datagram: a forger cannot add
 * a row per byte, written again every second
 */
TEST(TrafficCsvRows, ADatagramOfEveryTypeByteAddsARowPerSlotAtMost) {
  TrafficCounters counters;

  counters.countReceived(loopbackOnPort(PEER_PORT), datagramOfEveryTypeByte());

  const std::string rows = trafficCsvRowsOf(ONE_SECOND, counters.report());
  EXPECT_EQ(static_cast<std::size_t>(std::ranges::count(rows, '\n')),
            MESSAGE_SLOT_COUNT + 1);
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
  counters.follow(loopbackOnPort(PEER_PORT));
  counters.countSent(loopbackOnPort(PEER_PORT), datagram);
  counters.countReceived(loopbackOnPort(PEER_PORT), datagram);
  counters.unfollow(loopbackOnPort(PEER_PORT));
  counters.countSent(loopbackOnPort(PEER_PORT), datagram);
  counters.follow(loopbackOnPort(OTHER_PORT));
  counters.countSent(loopbackOnPort(OTHER_PORT), datagram);

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
