#include <gtest/gtest.h>
#include "ConnectedPlayer.hpp"
#include "EntityIdConstants.hpp"
#include "IGameReplication.hpp"
#include "MessageType.hpp"
#include "RtypeGameFixture.hpp"

namespace rtype::server::testing {

/**
 * Given a replication that was told about a world and a player in the roster
 * When the player joins
 * Then the player holds a ship of that world and is sent its spawn
 */
TEST_F(RtypeGameFixture, GameReplicationGivesAJoiningPlayerAShipOfTheWorld) {
  IGameReplication& replication = replicationOfTheWorld();
  ConnectedPlayer& player = join(0);

  replication.onPlayerJoined(player);

  EXPECT_NE(player.shipEntityId, identifiers::NO_ENTITY_ID);
  EXPECT_NE(world().shipPositionOf(player.shipEntityId), nullptr);
  expectFirstMessageSentTo(0, network::MessageType::EntitySpawned);
}

/**
 * Given a replication told about a world and a player in the roster
 * When a tick of the wait for players goes by
 * Then the player is sent the room
 */
TEST_F(RtypeGameFixture, GameReplicationTellsTheRoomOnAWaitingTick) {
  IGameReplication& replication = replicationOfTheWorld();
  join(0);

  replication.onWaitingTick();

  expectFirstMessageSentTo(0, network::MessageType::LobbyState);
}

/**
 * Given a replication told that the game ended by itself
 * When a tick after the end goes by
 * Then the player is sent the result
 */
TEST_F(RtypeGameFixture, GameReplicationTellsTheResultOnceTheGameEnded) {
  IGameReplication& replication = replicationOfTheWorld();
  join(0);
  replication.onGameFinished();

  replication.onFinishedTick();

  expectFirstMessageSentTo(0, network::MessageType::GameEnded);
}

/**
 * Given a replication whose game was ended without a result
 * When a tick after the end goes by
 * Then nothing is sent
 */
TEST_F(RtypeGameFixture, GameReplicationTellsNoResultToAGameEndedWithoutOne) {
  IGameReplication& replication = replicationOfTheWorld();
  join(0);

  replication.onFinishedTick();

  EXPECT_TRUE(sentDatagrams().empty());
}

/**
 * Given a replication told that the game ended, then told about a new world
 * When a tick after the end goes by
 * Then nothing is sent, since the end of the previous game is forgotten
 */
TEST_F(RtypeGameFixture, GameReplicationForgetsTheEndOfThePreviousGame) {
  IGameReplication& replication = replicationOfTheWorld();
  join(0);
  replication.onGameFinished();
  replication.onWorldBuilt(world());

  replication.onFinishedTick();

  EXPECT_TRUE(sentDatagrams().empty());
}

/**
 * Given a replication told about a world and a player in the roster
 * When a tick of the game in progress goes by
 * Then the player is sent the world
 */
TEST_F(RtypeGameFixture, GameReplicationSendsTheWorldOnAProgressTick) {
  IGameReplication& replication = replicationOfTheWorld();
  join(0);

  replication.onProgressTick(0, world().components());

  expectFirstMessageSentTo(0, network::MessageType::WorldState);
}

}  // namespace rtype::server::testing
