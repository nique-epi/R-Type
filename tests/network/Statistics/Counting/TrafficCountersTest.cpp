#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <span>
#include <thread>
#include <vector>
#include "DatagramCodec.hpp"
#include "DirectionTraffic.hpp"
#include "MessageSlotConstants.hpp"
#include "MessageType.hpp"
#include "PacketHeader.hpp"
#include "ProtocolConstants.hpp"
#include "TrafficCounters.hpp"
#include "TrafficFigures.hpp"
#include "TrafficFixtures.hpp"
#include "TrafficReport.hpp"
#include "TrafficTestConstants.hpp"
#include "TrafficTotals.hpp"

using rtype::network::DatagramCodec;
using rtype::network::DirectionTraffic;
using rtype::network::MESSAGE_SLOT_COUNT;
using rtype::network::MessageType;
using rtype::network::PacketHeader;
using rtype::network::PROTOCOL_VERSION;
using rtype::network::TrafficCounters;
using rtype::network::TrafficFigures;
using rtype::network::TrafficReport;
using rtype::network::UNKNOWN_MESSAGE_SLOT;
using rtype::network::testing::datagramHolding;
using rtype::network::testing::datagramOfTypeByte;
using rtype::network::testing::figuresOf;
using rtype::network::testing::loopbackOnPort;
using rtype::network::testing::withMessage;

/**
 * Given counters
 * When a datagram of one message is sent
 * Then the datagram counts once, with its header, its prefix and its payload
 */
TEST(TrafficCounters, CountsASentDatagramWithAllItsBytes) {
  TrafficCounters counters;

  counters.countSent(loopbackOnPort(PEER_PORT),
                     datagramHolding(MessageType::WorldState, LARGE_PAYLOAD));

  EXPECT_EQ(
      counters.report().others.sent.datagrams,
      (TrafficFigures{.count = 1,
                      .bytes = HEADER_BYTES + PREFIX_BYTES + LARGE_PAYLOAD}));
}

/**
 * Given a datagram holding two messages of two types
 * When it is sent
 * Then each message counts under its type, with its prefix and its payload,
 * and the header is in neither
 */
TEST(TrafficCounters, CountsEachMessageUnderItsTypeWithoutTheHeader) {
  TrafficCounters counters;
  const std::vector<std::byte> datagram =
      withMessage(datagramHolding(MessageType::WorldState, LARGE_PAYLOAD),
                  MessageType::GameEvent, SMALL_PAYLOAD);

  counters.countSent(loopbackOnPort(PEER_PORT), datagram);

  const DirectionTraffic sent = counters.report().others.sent;
  EXPECT_EQ(sent.datagrams.count, 1U);
  EXPECT_EQ(
      figuresOf(sent, MessageType::WorldState),
      (TrafficFigures{.count = 1, .bytes = PREFIX_BYTES + LARGE_PAYLOAD}));
  EXPECT_EQ(
      figuresOf(sent, MessageType::GameEvent),
      (TrafficFigures{.count = 1, .bytes = PREFIX_BYTES + SMALL_PAYLOAD}));
  EXPECT_EQ(sent.unreadable, TrafficFigures{});
}

/**
 * Given a valid datagram cut before its first message ends: empty, one byte, a
 * byte short of a header, a header alone
 * When it is received
 * Then it counts as an unreadable datagram of that size, and nothing throws
 */
TEST(TrafficCounters, ADatagramTooShortForAMessageIsUnreadable) {
  const std::vector<std::byte> whole =
      datagramHolding(MessageType::WorldState, LARGE_PAYLOAD);
  for (const std::size_t size : TOO_SHORT_SIZES) {
    SCOPED_TRACE(size);
    TrafficCounters counters;

    counters.countReceived(loopbackOnPort(PEER_PORT),
                           std::span<const std::byte>{whole}.first(size));

    const DirectionTraffic received = counters.report().others.received;
    EXPECT_EQ(received.unreadable, (TrafficFigures{.count = 1, .bytes = size}));
    EXPECT_EQ(figuresOf(received, MessageType::WorldState), TrafficFigures{});
  }
}

/**
 * Given a well framed datagram of another version of the protocol
 * When it is received
 * Then it is unreadable: what its bytes mean is not known
 */
TEST(TrafficCounters, ADatagramOfAnotherVersionIsUnreadable) {
  TrafficCounters counters;
  PacketHeader header;
  header.version = static_cast<std::uint8_t>(PROTOCOL_VERSION + 1);

  counters.countReceived(
      loopbackOnPort(PEER_PORT),
      DatagramCodec::encode(header, MessageType::WorldState,
                            std::vector<std::byte>(LARGE_PAYLOAD)));

  const DirectionTraffic received = counters.report().others.received;
  EXPECT_EQ(received.unreadable.count, 1U);
  EXPECT_EQ(figuresOf(received, MessageType::WorldState), TrafficFigures{});
}

/**
 * Given a whole message, then a message whose size runs past the end
 * When the datagram is received
 * Then even the whole message is not counted: the datagram is refused whole
 */
TEST(TrafficCounters, ASizeRunningPastTheEndCountsNoMessageAtAll) {
  TrafficCounters counters;
  std::vector<std::byte> datagram =
      withMessage(datagramHolding(MessageType::WorldState, LARGE_PAYLOAD),
                  MessageType::GameEvent, SMALL_PAYLOAD);
  datagram.pop_back();

  counters.countReceived(loopbackOnPort(PEER_PORT), datagram);

  const DirectionTraffic received = counters.report().others.received;
  EXPECT_EQ(received.unreadable.count, 1U);
  EXPECT_EQ(figuresOf(received, MessageType::WorldState), TrafficFigures{});
}

/**
 * Given messages of type bytes no message has: 0, the first byte after the last
 * message, and the largest
 * When they are received
 * Then they count together, in the one slot of the unknown messages
 */
TEST(TrafficCounters, EveryTypeByteNoMessageHasCountsInOneSlot) {
  TrafficCounters counters;

  for (const std::uint8_t typeByte :
       {NO_MESSAGE_TYPE_ZERO, static_cast<std::uint8_t>(MESSAGE_SLOT_COUNT),
        LARGEST_TYPE_BYTE}) {
    counters.countReceived(loopbackOnPort(PEER_PORT),
                           datagramOfTypeByte(typeByte, SMALL_PAYLOAD));
  }

  EXPECT_EQ(counters.report().others.received.messages[UNKNOWN_MESSAGE_SLOT],
            (TrafficFigures{.count = 3,
                            .bytes = 3 * (PREFIX_BYTES + SMALL_PAYLOAD)}));
}

/**
 * Given the last message of the protocol
 * When it is received
 * Then it counts under its own type, not with the unknown messages
 */
TEST(TrafficCounters, TheLastMessageOfTheProtocolHasItsOwnSlot) {
  TrafficCounters counters;

  counters.countReceived(
      loopbackOnPort(PEER_PORT),
      datagramHolding(MessageType::RoundTrips, SMALL_PAYLOAD));

  const DirectionTraffic received = counters.report().others.received;
  EXPECT_EQ(figuresOf(received, MessageType::RoundTrips).count, 1U);
  EXPECT_EQ(received.messages[UNKNOWN_MESSAGE_SLOT], TrafficFigures{});
}

/**
 * Given a followed peer
 * When a datagram goes each way with it, and another peer sends one
 * Then the row of the peer holds its two datagrams, and the others the third
 */
TEST(TrafficCounters, AFollowedPeerHasARowOfItsOwn) {
  TrafficCounters counters;
  const std::vector<std::byte> datagram =
      datagramHolding(MessageType::Heartbeat, SMALL_PAYLOAD);
  counters.follow(loopbackOnPort(PEER_PORT));

  counters.countSent(loopbackOnPort(PEER_PORT), datagram);
  counters.countReceived(loopbackOnPort(PEER_PORT), datagram);
  counters.countReceived(loopbackOnPort(OTHER_PORT), datagram);

  const TrafficReport report = counters.report();
  ASSERT_EQ(report.peers.size(), 1U);
  EXPECT_EQ(report.peers.front().peer, loopbackOnPort(PEER_PORT));
  EXPECT_EQ(report.peers.front().traffic.sent.datagrams.count, 1U);
  EXPECT_EQ(report.peers.front().traffic.received.datagrams.count, 1U);
  EXPECT_EQ(report.others.sent.datagrams.count, 0U);
  EXPECT_EQ(report.others.received.datagrams.count, 1U);
}

/**
 * Given a followed peer that sent a datagram
 * When it is followed again
 * Then it keeps one row, and what it counted
 */
TEST(TrafficCounters, FollowingAPeerAgainKeepsItsRow) {
  TrafficCounters counters;
  counters.follow(loopbackOnPort(PEER_PORT));
  counters.countReceived(
      loopbackOnPort(PEER_PORT),
      datagramHolding(MessageType::Heartbeat, SMALL_PAYLOAD));

  counters.follow(loopbackOnPort(PEER_PORT));

  const TrafficReport report = counters.report();
  ASSERT_EQ(report.peers.size(), 1U);
  EXPECT_EQ(report.peers.front().traffic.received.datagrams.count, 1U);
}

/**
 * Given a followed peer that exchanged datagrams
 * When it is no longer followed
 * Then its row is gone, the departed hold what it counted, the others nothing,
 * and the totals did not move
 */
TEST(TrafficCounters, UnfollowingMovesARowToTheDepartedAndLeavesTheTotals) {
  TrafficCounters counters;
  const std::vector<std::byte> datagram =
      datagramHolding(MessageType::WorldState, LARGE_PAYLOAD);
  counters.follow(loopbackOnPort(PEER_PORT));
  counters.countSent(loopbackOnPort(PEER_PORT), datagram);
  counters.countReceived(loopbackOnPort(PEER_PORT), datagram);
  const rtype::network::TrafficTotals before =
      trafficTotalsOf(counters.report());

  counters.unfollow(loopbackOnPort(PEER_PORT));

  const TrafficReport report = counters.report();
  EXPECT_TRUE(report.peers.empty());
  EXPECT_EQ(report.departed.sent.datagrams.count, 1U);
  EXPECT_EQ(figuresOf(report.departed.received, MessageType::WorldState).count,
            1U);
  EXPECT_EQ(report.others.sent.datagrams.count, 0U);
  EXPECT_EQ(trafficTotalsOf(report), before);
}

/**
 * Given a peer nobody follows
 * When it is unfollowed
 * Then nothing changes
 */
TEST(TrafficCounters, UnfollowingAStrangerChangesNothing) {
  TrafficCounters counters;

  counters.unfollow(loopbackOnPort(PEER_PORT));

  const TrafficReport report = counters.report();
  EXPECT_TRUE(report.peers.empty());
  EXPECT_EQ(report.departed.sent.datagrams.count, 0U);
}

/**
 * Given a flood from a thousand senders nobody follows
 * When each sends a datagram
 * Then no row is added, whatever they claim, and the others count them all
 */
TEST(TrafficCounters, AFloodOfStrangersAddsNoRow) {
  TrafficCounters counters;
  const std::vector<std::byte> datagram =
      datagramHolding(MessageType::Heartbeat, SMALL_PAYLOAD);

  for (std::uint16_t port = 1; port <= FLOOD_SENDERS; ++port) {
    counters.countReceived(loopbackOnPort(port), datagram);
  }

  const TrafficReport report = counters.report();
  EXPECT_TRUE(report.peers.empty());
  EXPECT_EQ(report.others.received.datagrams.count, FLOOD_SENDERS);
}

/**
 * Given a thread that counts datagrams of one size
 * When another thread reads reports meanwhile
 * Then each report holds whole datagrams: its bytes are its count times the
 * size
 */
TEST(TrafficCounters, AReportReadWhileCountingHoldsWholeDatagrams) {
  TrafficCounters counters;
  const std::vector<std::byte> datagram =
      datagramHolding(MessageType::WorldState, LARGE_PAYLOAD);
  const std::jthread writer{[&counters, &datagram] {
    for (std::size_t sent = 0; sent < DATAGRAMS_WHILE_READING; ++sent) {
      counters.countSent(loopbackOnPort(PEER_PORT), datagram);
    }
  }};

  for (std::size_t read = 0; read < REPORTS_WHILE_COUNTING; ++read) {
    const TrafficFigures sent = counters.report().others.sent.datagrams;
    EXPECT_EQ(sent.bytes, sent.count * datagram.size());
  }
}
