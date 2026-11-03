#include <gtest/gtest.h>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Heartbeats.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "SentDatagrams.hpp"
#include "SessionsFixtures.hpp"
#include "SessionsTestConstants.hpp"
#include "TimeConstants.hpp"
#include "TimeTestConstants.hpp"

namespace rtype::server {

using testing::addPlayer;
using testing::onlyMessageTypeOf;
using testing::PROTOCOL_HEARTBEAT_SILENCE;
using testing::sentTo;
using testing::SOME_STALL;
using testing::SOME_TICK_TIME;

/**
 * Given two players, the server sent the first a datagram in the previous
 * tick and the second one tick earlier
 * When a tick starts SOME_TICK_TIME after the previous one
 * Then the first was sent nothing for that time, and the second for twice
 * that time
 */
TEST(Heartbeats, AddsTheTimeOfTheTickToEveryPlayer) {
  PlayerRoster roster;
  const ConnectedPlayer& sentLastTick = addPlayer(roster, 0);
  ConnectedPlayer& sentTheTickBefore = addPlayer(roster, 1);
  sentTheTickBefore.sinceSent = SOME_TICK_TIME;

  addTimeSinceSent(roster, SOME_TICK_TIME);

  EXPECT_EQ(sentLastTick.sinceSent, SOME_TICK_TIME);
  EXPECT_EQ(sentTheTickBefore.sinceSent, 2 * SOME_TICK_TIME);
}

/**
 * Given a player the server sent nothing for one nanosecond less than the
 * 60 whole ticks that fit in the 1 s of the protocol
 * When the heartbeats are sent
 * Then no Heartbeat goes out
 */
TEST(Heartbeats, SendsNothingBeforeTheInterval) {
  PlayerRoster roster;
  ConnectedPlayer& player = addPlayer(roster);
  player.sinceSent = PROTOCOL_HEARTBEAT_SILENCE - ONE_NANOSECOND;
  std::vector<network::OutgoingDatagram> sent;

  sendHeartbeats(roster, [&sent](network::OutgoingDatagram datagram) {
    sent.push_back(std::move(datagram));
  });

  EXPECT_TRUE(sent.empty());
}

/**
 * Given a player the server sent nothing for the 60 whole ticks that fit in
 * the 1 s of the protocol
 * When the heartbeats are sent
 * Then it is sent one Heartbeat, and the time since it was sent a datagram
 * starts again from zero
 */
TEST(Heartbeats, SendsAHeartbeatOnceTheIntervalIsReached) {
  PlayerRoster roster;
  ConnectedPlayer& player = addPlayer(roster);
  player.sinceSent = PROTOCOL_HEARTBEAT_SILENCE;
  std::vector<network::OutgoingDatagram> sent;

  sendHeartbeats(roster, [&sent](network::OutgoingDatagram datagram) {
    sent.push_back(std::move(datagram));
  });

  ASSERT_EQ(sent.size(), 1U);
  EXPECT_EQ(sent.front().destination, player.endpoint);
  EXPECT_EQ(onlyMessageTypeOf(sent.front()), network::MessageType::Heartbeat);
  EXPECT_EQ(player.sinceSent, engine::Duration::zero());
}

/**
 * Given two players the server sent a datagram just before it stalled
 * When a tick starts SOME_STALL after the previous one, then the heartbeats
 * are sent
 * Then each player is sent one Heartbeat: its client counted the stall too
 */
TEST(Heartbeats, SendsEveryPlayerAHeartbeatAfterAStall) {
  PlayerRoster roster;
  const ConnectedPlayer& first = addPlayer(roster, 0);
  const ConnectedPlayer& second = addPlayer(roster, 1);
  std::vector<network::OutgoingDatagram> sent;

  addTimeSinceSent(roster, SOME_STALL);
  sendHeartbeats(roster, [&sent](network::OutgoingDatagram datagram) {
    sent.push_back(std::move(datagram));
  });

  EXPECT_EQ(sentTo(sent, first.endpoint).size(), 1U);
  EXPECT_EQ(sentTo(sent, second.endpoint).size(), 1U);
}

}  // namespace rtype::server
