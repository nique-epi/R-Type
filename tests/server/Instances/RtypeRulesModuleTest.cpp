#include <gtest/gtest.h>
#include "ConnectedPlayer.hpp"
#include "EnemyKilled.hpp"
#include "GameModuleFixture.hpp"
#include "GameModuleTestConstants.hpp"
#include "GameState.hpp"
#include "IGameFlow.hpp"
#include "MessageType.hpp"
#include "ReplicationModule.hpp"
#include "RtypeRulesModule.hpp"
#include "SentDatagrams.hpp"

namespace rtype::server::testing {

/**
 * Given the replication module installed, and the rules of R-Type
 * When the rules are installed
 * Then the game offers the IGameFlow service
 */
TEST_F(GameModuleFixture, RtypeRulesModuleProvidesTheFlowWhenInstalled) {
  ReplicationModule replication;
  RtypeRulesModule rules;
  install(replication);

  install(rules);

  EXPECT_NE(provided<IGameFlow>(), nullptr);
}

/**
 * Given both modules installed
 * When they are started
 * Then the game waits for players, at tick 0
 */
TEST_F(GameModuleFixture, RtypeRulesModuleStartsAGameThatWaitsForPlayers) {
  ReplicationModule replication;
  RtypeRulesModule rules;
  install(replication);
  install(rules);

  replication.start();
  rules.start();

  EXPECT_EQ(provided<IGameFlow>()->state(), GameState::WaitingForPlayers);
  EXPECT_EQ(provided<IGameFlow>()->tick(), 0U);
}

/**
 * Given both modules started and a player who has not said it is ready
 * When the stages of a tick run
 * Then the player is sent the room
 */
TEST_F(GameModuleFixture, RtypeRulesModuleTellsTheRoomThroughTheReplication) {
  ReplicationModule replication;
  RtypeRulesModule rules;
  install(replication);
  install(rules);
  replication.start();
  rules.start();
  join(0);

  runStages();

  const auto received = sentTo(sentDatagrams(), playerEndpoint(0));
  ASSERT_FALSE(received.empty());
  EXPECT_EQ(onlyMessageTypeOf(received.front()),
            network::MessageType::LobbyState);
}

/**
 * Given both modules started and a connected player
 * When an enemy killed by that player is published and delivered
 * Then the player has the points of the enemy
 */
TEST_F(GameModuleFixture, RtypeRulesModuleCreditsTheKillsToTheScore) {
  ReplicationModule replication;
  RtypeRulesModule rules;
  install(replication);
  install(rules);
  replication.start();
  rules.start();
  const ConnectedPlayer& player = join(0);

  publishAndDispatch(game::EnemyKilled{.playerNumber = player.playerNumber,
                                       .points = ENEMY_POINTS});

  EXPECT_EQ(player.score, ENEMY_POINTS);
}

/**
 * Given both modules started and a connected player
 * When the modules are stopped, then a kill by that player is published and
 * delivered
 * Then the score of the player is unchanged
 */
TEST_F(GameModuleFixture, RtypeRulesModuleStopsCountingOnceStopped) {
  ReplicationModule replication;
  RtypeRulesModule rules;
  install(replication);
  install(rules);
  replication.start();
  rules.start();
  const ConnectedPlayer& player = join(0);
  rules.stop();
  replication.stop();

  publishAndDispatch(game::EnemyKilled{.playerNumber = player.playerNumber,
                                       .points = ENEMY_POINTS});

  EXPECT_EQ(player.score, 0U);
}

/**
 * Given both modules started and a player in the roster with no ship
 * When the functions added to the hooks for a joined player are called
 * Then the player holds a ship of the world
 */
TEST_F(GameModuleFixture, RtypeRulesModuleGivesAJoiningPlayerAShip) {
  ReplicationModule replication;
  RtypeRulesModule rules;
  install(replication);
  install(rules);
  replication.start();
  rules.start();
  ConnectedPlayer& player = join(0);

  playerJoined(player);

  EXPECT_NE(world().shipPositionOf(player.shipEntityId), nullptr);
}

/**
 * Given both modules started and a player who holds a ship
 * When the functions added to the hooks for a closing session are called
 * Then the ship is no longer in the world
 */
TEST_F(GameModuleFixture, RtypeRulesModuleRemovesTheShipOfAClosingSession) {
  ReplicationModule replication;
  RtypeRulesModule rules;
  install(replication);
  install(rules);
  replication.start();
  rules.start();
  ConnectedPlayer& player = join(0);
  playerJoined(player);

  sessionClosing(player);

  EXPECT_EQ(world().shipPositionOf(player.shipEntityId), nullptr);
}

}  // namespace rtype::server::testing
