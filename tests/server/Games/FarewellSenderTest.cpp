#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "DatagramCodec.hpp"
#include "Disconnection.hpp"
#include "ModuleConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "RunningGameFixture.hpp"
#include "SentDatagrams.hpp"
#include "ServerModuleTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::network::DatagramCodec;
using rtype::network::Disconnection;
using rtype::network::OutgoingDatagram;
using rtype::network::PacketHeader;
using rtype::server::FAREWELL_COPIES;
using rtype::server::FAREWELL_COPY_INTERVAL;
using rtype::server::FAREWELL_FLUSH_WAIT;
using rtype::server::testing::onlyMessageIn;
using rtype::server::testing::playerEndpoint;
using rtype::server::testing::sentTo;

/**
 * Given a game with two players
 * When the server says goodbye
 * Then each player gets FAREWELL_COPIES datagrams, each one a
 * Disconnection
 */
TEST_F(RunningGameFixture, SendsEveryPlayerAllTheCopiesOfADisconnection) {
  hostGame();
  join(0);
  join(1);

  farewell().sayGoodbyeTo(roster());

  for (const std::size_t index : {0U, 1U}) {
    const std::vector<OutgoingDatagram> received =
        sentTo(sender().sent(), playerEndpoint(index));
    ASSERT_EQ(received.size(), FAREWELL_COPIES);
    for (const OutgoingDatagram& datagram : received) {
      static_cast<void>(onlyMessageIn<Disconnection>(datagram));
    }
  }
}

/**
 * Given a game with a player
 * When the server says goodbye
 * Then the copies carry the token of the player and the three sequences that
 * follow the ones already used: each copy takes the next sequence of its
 * session
 */
TEST_F(RunningGameFixture, WritesTheCopiesUnderTheSessionHeaderOfThePlayer) {
  hostGame();
  const std::uint16_t firstFreeSequence = join(0).nextSequence;

  farewell().sayGoodbyeTo(roster());

  const std::vector<OutgoingDatagram> received =
      sentTo(sender().sent(), playerEndpoint(0));
  ASSERT_EQ(received.size(), FAREWELL_COPIES);
  for (std::size_t copy = 0; copy < received.size(); ++copy) {
    const PacketHeader header =
        DatagramCodec::decodeHeader(received[copy].payload);
    EXPECT_EQ(header.sessionToken, PLAYER_TOKEN_BASE);
    EXPECT_EQ(header.sequence, firstFreeSequence + copy);
  }
}

/**
 * Given a game with a player
 * When the server says goodbye
 * Then it waits FAREWELL_COPY_INTERVAL between two copies, then
 * FAREWELL_FLUSH_WAIT after the last one
 */
TEST_F(RunningGameFixture, WaitsBetweenTheCopiesThenForTheLastOneToLeave) {
  hostGame();
  join(0);

  farewell().sayGoodbyeTo(roster());

  EXPECT_EQ(pauses(), (std::vector<rtype::engine::Duration>{
                          FAREWELL_COPY_INTERVAL, FAREWELL_COPY_INTERVAL,
                          FAREWELL_FLUSH_WAIT}));
}

/**
 * Given a game without a player
 * When the server says goodbye
 * Then nothing is sent and the server does not wait
 */
TEST_F(RunningGameFixture, SendsNothingAndWaitsForNothingWhenNoPlayerIsThere) {
  hostGame();

  farewell().sayGoodbyeTo(roster());

  EXPECT_TRUE(sender().sent().empty());
  EXPECT_TRUE(pauses().empty());
}
