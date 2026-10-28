#include <gtest/gtest.h>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "Heartbeats.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "SentDatagrams.hpp"
#include "SessionConstants.hpp"
#include "SessionsFixtures.hpp"

namespace rtype::server {
namespace {

using testing::addPlayer;
using testing::onlyMessageTypeOf;

/**
 * Given a player sent nothing for one tick less than TICKS_PER_HEARTBEAT
 * When the tick ends
 * Then no Heartbeat is sent and the tick is counted
 */
TEST(Heartbeats, SendsNothingBeforeTheInterval) {
  PlayerRoster roster;
  ConnectedPlayer& player = addPlayer(roster);
  player.ticksSinceSent = TICKS_PER_HEARTBEAT - 1;
  std::vector<network::OutgoingDatagram> sent;

  sendHeartbeats(roster, [&sent](network::OutgoingDatagram datagram) {
    sent.push_back(std::move(datagram));
  });

  EXPECT_TRUE(sent.empty());
  EXPECT_EQ(player.ticksSinceSent, TICKS_PER_HEARTBEAT);
}

/**
 * Given a player sent nothing for TICKS_PER_HEARTBEAT ticks
 * When the tick ends
 * Then it is sent one Heartbeat, and the count starts again
 */
TEST(Heartbeats, SendsAHeartbeatOnceTheIntervalIsReached) {
  PlayerRoster roster;
  ConnectedPlayer& player = addPlayer(roster);
  player.ticksSinceSent = TICKS_PER_HEARTBEAT;
  std::vector<network::OutgoingDatagram> sent;

  sendHeartbeats(roster, [&sent](network::OutgoingDatagram datagram) {
    sent.push_back(std::move(datagram));
  });

  ASSERT_EQ(sent.size(), 1U);
  EXPECT_EQ(sent.front().destination, player.endpoint);
  EXPECT_EQ(onlyMessageTypeOf(sent.front()), network::MessageType::Heartbeat);
  EXPECT_EQ(player.ticksSinceSent, 1U);
}

}  // namespace
}  // namespace rtype::server
