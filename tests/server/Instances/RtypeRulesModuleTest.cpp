#include <gtest/gtest.h>
#include "ConnectedPlayer.hpp"
#include "EnemyKilled.hpp"
#include "GameModuleTestConstants.hpp"
#include "GameState.hpp"
#include "IGameFlow.hpp"
#include "MessageType.hpp"
#include "RtypeGameFixture.hpp"

namespace rtype::server::testing {

/**
 * Given the replication module installed, and the rules of R-Type
 * When the rules are installed
 * Then the game offers the IGameFlow service, answering for a game that waits
 * for players at tick 0 even before the rules start
 */
TEST_F(RtypeGameFixture, RtypeRulesModuleProvidesTheFlowWhenInstalled) {
  installReplication();

  install(rules());

  ASSERT_NE(provided<IGameFlow>(), nullptr);
  EXPECT_EQ(provided<IGameFlow>()->state(), GameState::WaitingForPlayers);
  EXPECT_EQ(provided<IGameFlow>()->tick(), 0U);
}

/**
 * Given both modules started
 * When the flow is asked its state
 * Then the game waits for players, at tick 0
 */
TEST_F(RtypeGameFixture, RtypeRulesModuleStartsAGameThatWaitsForPlayers) {
  startBoth();

  EXPECT_EQ(provided<IGameFlow>()->state(), GameState::WaitingForPlayers);
  EXPECT_EQ(provided<IGameFlow>()->tick(), 0U);
}

/**
 * Given both modules started and a player who has not said it is ready
 * When the stages of a tick run
 * Then the player is sent the room
 */
TEST_F(RtypeGameFixture, RtypeRulesModuleTellsTheRoomThroughTheReplication) {
  startBoth();
  join(0);

  runStages();

  expectFirstMessageSentTo(0, network::MessageType::LobbyState);
}

/**
 * Given both modules started and a connected player
 * When an enemy killed by that player is published and delivered
 * Then the player has the points of the enemy
 */
TEST_F(RtypeGameFixture, RtypeRulesModuleCreditsTheKillsToTheScore) {
  startBoth();
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
TEST_F(RtypeGameFixture, RtypeRulesModuleStopsCountingOnceStopped) {
  startBoth();
  const ConnectedPlayer& player = join(0);
  stopBoth();

  publishAndDispatch(game::EnemyKilled{.playerNumber = player.playerNumber,
                                       .points = ENEMY_POINTS});

  EXPECT_EQ(player.score, 0U);
}

/**
 * Given both modules started and a player in the roster with no ship
 * When the functions added to the hooks for a joined player are called
 * Then the player holds a ship of the world
 */
TEST_F(RtypeGameFixture, RtypeRulesModuleGivesAJoiningPlayerAShip) {
  startBoth();
  ConnectedPlayer& player = join(0);

  playerJoined(player);

  EXPECT_NE(rules().world().shipPositionOf(player.shipEntityId), nullptr);
}

/**
 * Given both modules started and a player who holds a ship
 * When the functions added to the hooks for a closing session are called
 * Then the ship is no longer in the world
 */
TEST_F(RtypeGameFixture, RtypeRulesModuleRemovesTheShipOfAClosingSession) {
  startBoth();
  ConnectedPlayer& player = join(0);
  playerJoined(player);

  sessionClosing(player);

  EXPECT_EQ(rules().world().shipPositionOf(player.shipEntityId), nullptr);
}

}  // namespace rtype::server::testing
