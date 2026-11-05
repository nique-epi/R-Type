#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "EntityIdConstants.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntitySpawned.hpp"
#include "EntityTypesTestConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShipSpawnAnnouncerFixture.hpp"
#include "ShipStartPosition.hpp"
#include "ShippedEntitySheets.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * Given an empty game
 * When the first player joins
 * Then its ship is in the world at the start position of player 1 and the
 * player is sent that one EntitySpawned
 */
TEST_F(ShipSpawnAnnouncerFixture, TheFirstPlayerIsToldAboutItsOwnShip) {
  const ConnectedPlayer& player = join(0);

  EXPECT_EQ(world().entityCount(), 1U);
  EXPECT_NE(player.shipEntityId, identifiers::NO_ENTITY_ID);
  ASSERT_EQ(sent().size(), 1U);
  const auto spawned = onlyMessageIn<network::EntitySpawned>(sent().front());
  const game::Position expected =
      game::shipStartPosition(1, network::MAXIMUM_PLAYERS);
  EXPECT_EQ(spawned.entityId, player.shipEntityId);
  EXPECT_EQ(spawned.entityType, game::testing::shippedEntitySheets()
                                    .named(game::testing::SHIP_SHEET)
                                    .number);
  EXPECT_EQ(spawned.x, expected.x);
  EXPECT_EQ(spawned.y, expected.y);
}

/**
 * Given player 1 already in the game
 * When player 2 joins
 * Then player 1 is sent the ship of player 2 at the start position of
 * player 2, and player 2 is sent its own ship and the ship of player 1
 */
TEST_F(ShipSpawnAnnouncerFixture,
       TheOthersAreToldAboutANewShipAndItSeesTheirs) {
  const ConnectedPlayer& first = join(0);
  const ConnectedPlayer& second = join(1);

  const std::vector<network::OutgoingDatagram> toFirst = sentTo(0);
  ASSERT_EQ(toFirst.size(), 2U);
  const auto secondShip = onlyMessageIn<network::EntitySpawned>(toFirst.back());
  const game::Position expected =
      game::shipStartPosition(2, network::MAXIMUM_PLAYERS);
  EXPECT_EQ(secondShip.entityId, second.shipEntityId);
  EXPECT_EQ(secondShip.x, expected.x);
  EXPECT_EQ(secondShip.y, expected.y);
  const std::vector<network::OutgoingDatagram> toSecond = sentTo(1);
  ASSERT_EQ(toSecond.size(), 2U);
  const std::set<std::uint32_t> shipsSeenBySecond{
      onlyMessageIn<network::EntitySpawned>(toSecond.front()).entityId,
      onlyMessageIn<network::EntitySpawned>(toSecond.back()).entityId};
  EXPECT_EQ(shipsSeenBySecond,
            (std::set<std::uint32_t>{first.shipEntityId, second.shipEntityId}));
  EXPECT_NE(first.shipEntityId, second.shipEntityId);
}

/**
 * Given four players in the game
 * When the ships of players 1 to 4 are compared
 * Then no two ships share a position
 */
TEST_F(ShipSpawnAnnouncerFixture, FourPlayersGetFourDistinctShips) {
  for (std::size_t index = 0; index < network::MAXIMUM_PLAYERS; ++index) {
    join(index);
  }

  EXPECT_EQ(world().entityCount(), network::MAXIMUM_PLAYERS);
  const std::vector<network::OutgoingDatagram> toFirst = sentTo(0);
  ASSERT_EQ(toFirst.size(), network::MAXIMUM_PLAYERS);
  for (std::size_t first = 0; first < toFirst.size(); ++first) {
    for (std::size_t second = first + 1; second < toFirst.size(); ++second) {
      EXPECT_NE(onlyMessageIn<network::EntitySpawned>(toFirst.at(first)).y,
                onlyMessageIn<network::EntitySpawned>(toFirst.at(second)).y);
    }
  }
}

/**
 * Given a player who already has a ship
 * When the announcer is told again that it joined
 * Then no second ship is created and nothing is sent
 */
TEST_F(ShipSpawnAnnouncerFixture, APlayerWithAShipGetsNoSecondOne) {
  const ConnectedPlayer& player = join(0);
  const std::size_t sentBefore = sent().size();
  const std::uint32_t shipBefore = player.shipEntityId;

  // join() adds the same endpoint again: the roster returns the same player.
  join(0);

  EXPECT_EQ(world().entityCount(), 1U);
  EXPECT_EQ(player.shipEntityId, shipBefore);
  EXPECT_EQ(sent().size(), sentBefore);
}

/**
 * Given player 1 who was sent one datagram
 * When player 2 joins and player 1 is sent a second one
 * Then the sequence of the datagrams to player 1 goes 0, 1 and carries its
 * session token
 */
TEST_F(ShipSpawnAnnouncerFixture,
       DatagramsCarryTheSessionAndAdvanceTheSequence) {
  join(0);
  join(1);

  const std::vector<network::OutgoingDatagram> toFirst = sentTo(0);
  ASSERT_EQ(toFirst.size(), 2U);
  const network::PacketHeader first =
      network::DatagramCodec::decodeHeader(toFirst.front().payload);
  const network::PacketHeader second =
      network::DatagramCodec::decodeHeader(toFirst.back().payload);
  EXPECT_EQ(first.sessionToken, SESSION_TOKEN);
  EXPECT_EQ(first.sequence, 0);
  EXPECT_EQ(second.sequence, 1);
}

}  // namespace rtype::server::testing
