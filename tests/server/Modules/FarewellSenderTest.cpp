#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "DatagramCodec.hpp"
#include "Disconnection.hpp"
#include "GameEventConstants.hpp"
#include "GamesFixture.hpp"
#include "ModuleConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "SentDatagrams.hpp"
#include "ServerModuleTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::network::DatagramCodec;
using rtype::network::Disconnection;
using rtype::network::OutgoingDatagram;
using rtype::network::PacketHeader;
using rtype::server::FAREWELL_FLUSH_WAIT;
using rtype::server::REPEATED_MESSAGE_COPIES;
using rtype::server::REPEATED_MESSAGE_INTERVAL;
using rtype::server::testing::onlyMessageIn;
using rtype::server::testing::playerEndpoint;
using rtype::server::testing::sentTo;

/**
 * Given a game with two players
 * When the server says goodbye
 * Then each player gets REPEATED_MESSAGE_COPIES datagrams, each one a
 * Disconnection
 */
TEST_F(GamesFixture, SendsEveryPlayerAllTheCopiesOfADisconnection) {
  addGame();
  join(0);
  join(1);

  farewell().sayGoodbyeTo(games());

  for (const std::size_t index : {0U, 1U}) {
    const std::vector<OutgoingDatagram> received =
        sentTo(sender().sent(), playerEndpoint(index));
    ASSERT_EQ(received.size(), REPEATED_MESSAGE_COPIES);
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
TEST_F(GamesFixture, WritesTheCopiesUnderTheSessionHeaderOfThePlayer) {
  addGame();
  const std::uint16_t firstFreeSequence = join(0).nextSequence;

  farewell().sayGoodbyeTo(games());

  const std::vector<OutgoingDatagram> received =
      sentTo(sender().sent(), playerEndpoint(0));
  ASSERT_EQ(received.size(), REPEATED_MESSAGE_COPIES);
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
 * Then it waits REPEATED_MESSAGE_INTERVAL between two copies, then
 * FAREWELL_FLUSH_WAIT after the last one
 */
TEST_F(GamesFixture, WaitsBetweenTheCopiesThenForTheLastOneToLeave) {
  addGame();
  join(0);

  farewell().sayGoodbyeTo(games());

  EXPECT_EQ(pauses(), (std::vector<rtype::engine::Duration>{
                          REPEATED_MESSAGE_INTERVAL, REPEATED_MESSAGE_INTERVAL,
                          FAREWELL_FLUSH_WAIT}));
}

/**
 * Given a game without a player
 * When the server says goodbye
 * Then nothing is sent and the server does not wait
 */
TEST_F(GamesFixture, SendsNothingAndWaitsForNothingWhenNoPlayerIsThere) {
  addGame();

  farewell().sayGoodbyeTo(games());

  EXPECT_TRUE(sender().sent().empty());
  EXPECT_TRUE(pauses().empty());
}
