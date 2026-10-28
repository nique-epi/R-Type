#include <gtest/gtest.h>
#include "ConnectedPlayer.hpp"
#include "EntityIdConstants.hpp"
#include "GameModuleFixture.hpp"
#include "IGameReplication.hpp"
#include "MessageType.hpp"
#include "ReplicationModule.hpp"
#include "SentDatagrams.hpp"

namespace rtype::server::testing {

/**
 * Given a replication that was told about a world and a player in the roster
 * When the player joins
 * Then the player holds a ship of that world and is sent its spawn
 */
TEST_F(GameModuleFixture, GameReplicationGivesAJoiningPlayerAShipOfTheWorld) {
  ReplicationModule module;
  install(module);
  IGameReplication& replication = *provided<IGameReplication>();
  replication.onWorldBuilt(world());
  ConnectedPlayer& player = join(0);

  replication.onPlayerJoined(player);

  EXPECT_NE(player.shipEntityId, identifiers::NO_ENTITY_ID);
  EXPECT_NE(world().shipPositionOf(player.shipEntityId), nullptr);
  const auto received = sentTo(sentDatagrams(), playerEndpoint(0));
  ASSERT_FALSE(received.empty());
  EXPECT_EQ(onlyMessageTypeOf(received.front()),
            network::MessageType::EntitySpawned);
}

/**
 * Given a replication told about a world and a player in the roster
 * When a tick of the wait for players goes by
 * Then the player is sent the room
 */
TEST_F(GameModuleFixture, GameReplicationTellsTheRoomOnAWaitingTick) {
  ReplicationModule module;
  install(module);
  IGameReplication& replication = *provided<IGameReplication>();
  replication.onWorldBuilt(world());
  join(0);

  replication.onWaitingTick();

  const auto received = sentTo(sentDatagrams(), playerEndpoint(0));
  ASSERT_FALSE(received.empty());
  EXPECT_EQ(onlyMessageTypeOf(received.front()),
            network::MessageType::LobbyState);
}

/**
 * Given a replication told that the game ended by itself
 * When a tick after the end goes by
 * Then the player is sent the result
 */
TEST_F(GameModuleFixture, GameReplicationTellsTheResultOnceTheGameEnded) {
  ReplicationModule module;
  install(module);
  IGameReplication& replication = *provided<IGameReplication>();
  replication.onWorldBuilt(world());
  join(0);
  replication.onGameFinished();

  replication.onFinishedTick();

  const auto received = sentTo(sentDatagrams(), playerEndpoint(0));
  ASSERT_FALSE(received.empty());
  EXPECT_EQ(onlyMessageTypeOf(received.front()),
            network::MessageType::GameEnded);
}

/**
 * Given a replication whose game was ended without a result
 * When a tick after the end goes by
 * Then nothing is sent
 */
TEST_F(GameModuleFixture, GameReplicationTellsNoResultToAGameEndedWithoutOne) {
  ReplicationModule module;
  install(module);
  IGameReplication& replication = *provided<IGameReplication>();
  replication.onWorldBuilt(world());
  join(0);

  replication.onFinishedTick();

  EXPECT_TRUE(sentDatagrams().empty());
}

/**
 * Given a replication told that the game ended, then told about a new world
 * When a tick after the end goes by
 * Then nothing is sent, since the end of the previous game is forgotten
 */
TEST_F(GameModuleFixture, GameReplicationForgetsTheEndOfThePreviousGame) {
  ReplicationModule module;
  install(module);
  IGameReplication& replication = *provided<IGameReplication>();
  replication.onWorldBuilt(world());
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
TEST_F(GameModuleFixture, GameReplicationSendsTheWorldOnAProgressTick) {
  ReplicationModule module;
  install(module);
  IGameReplication& replication = *provided<IGameReplication>();
  replication.onWorldBuilt(world());
  join(0);

  replication.onProgressTick(0, world().components());

  const auto received = sentTo(sentDatagrams(), playerEndpoint(0));
  ASSERT_FALSE(received.empty());
  EXPECT_EQ(onlyMessageTypeOf(received.front()),
            network::MessageType::WorldState);
}

}  // namespace rtype::server::testing
