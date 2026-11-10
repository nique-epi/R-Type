#include <gtest/gtest.h>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "GameEnded.hpp"
#include "GameFlowLevelFixture.hpp"
#include "GameFlowLevelTestConstants.hpp"
#include "GameFlowTestConstants.hpp"
#include "GameState.hpp"
#include "Instances/GameInstanceTestConstants.hpp"
#include "PlayerRoster.hpp"
#include "SentDatagrams.hpp"
#include "game/Waves/WaveTestConstants.hpp"

using rtype::game::testing::TICKS_TO_THE_WHOLE_FIRST_WAVE;
using rtype::network::GameEnded;
using rtype::network::WireGameOutcome;
using rtype::server::ConnectedPlayer;
using rtype::server::GameState;
using rtype::server::testing::FIRST_PLAYER;
using rtype::server::testing::GameFlowBrokenLevelFixture;
using rtype::server::testing::GameFlowOneStageFixture;
using rtype::server::testing::GameFlowSendingLevelFixture;
using rtype::server::testing::GameFlowTwoStagesFixture;
using rtype::server::testing::LIVES_OF_THE_RULES;
using rtype::server::testing::playerEndpoint;
using rtype::server::testing::TICKS_BEFORE_THE_CHECKPOINT;
using rtype::server::testing::TICKS_PAST_THE_CHECKPOINT;

/**
 * Given a game that plays a level, before its first checkpoint
 * When its only player loses every life
 * Then the game is lost, as a game of waves is
 */
TEST_F(GameFlowTwoStagesFixture, IsLostBeforeTheFirstCheckpoint) {
  startGameWith({FIRST_PLAYER});
  runTicks(TICKS_BEFORE_THE_CHECKPOINT);
  takeEveryLifeOf(*roster().find(playerEndpoint(FIRST_PLAYER)));

  tick();

  EXPECT_EQ(flow().state(), GameState::Finished);
}

/**
 * Given a game that plays a level, past its first checkpoint
 * When its only player loses every life
 * Then the game goes on from the checkpoint
 */
TEST_F(GameFlowTwoStagesFixture, GoesOnFromTheCheckpointWhenTheTeamIsLost) {
  startGameWith({FIRST_PLAYER});
  runTicks(TICKS_PAST_THE_CHECKPOINT);
  takeEveryLifeOf(*roster().find(playerEndpoint(FIRST_PLAYER)));

  tick();

  EXPECT_EQ(flow().state(), GameState::InProgress);
}

/**
 * Given a game that plays a level, past its first checkpoint
 * When its only player loses every life
 * Then that player has the lives of the rules again
 */
TEST_F(GameFlowTwoStagesFixture, GivesTheLivesBackAtTheCheckpoint) {
  startGameWith({FIRST_PLAYER});
  runTicks(TICKS_PAST_THE_CHECKPOINT);
  const ConnectedPlayer& player = *roster().find(playerEndpoint(FIRST_PLAYER));
  takeEveryLifeOf(*roster().find(playerEndpoint(FIRST_PLAYER)));

  tick();

  EXPECT_EQ(livesLeftTo(player), LIVES_OF_THE_RULES);
}

/**
 * Given a game that plays a level, past its first checkpoint
 * When the last ship of its only player is destroyed
 * Then that player flies a new ship
 */
TEST_F(GameFlowTwoStagesFixture, GivesANewShipAtTheCheckpoint) {
  startGameWith({FIRST_PLAYER});
  runTicks(TICKS_PAST_THE_CHECKPOINT);
  ConnectedPlayer& player = *roster().find(playerEndpoint(FIRST_PLAYER));
  destroyTheLastShipOf(player);

  tick();

  EXPECT_NE(flow().world().shipPositionOf(player.shipEntityId), nullptr);
  EXPECT_EQ(livesLeftTo(player), LIVES_OF_THE_RULES);
}

/**
 * Given a game that plays a level
 * When the ticks that would bring a whole wave of Bydos run
 * Then no Bydo came: the level runs no wave
 */
TEST_F(GameFlowTwoStagesFixture, RunsNoWave) {
  startGameWith({FIRST_PLAYER});

  runTicks(TICKS_TO_THE_WHOLE_FIRST_WAVE);

  EXPECT_EQ(enemiesInPlay(), 0U);
}

/**
 * Given a game that plays a level whose stage creates a Drifter
 * When the first tick of the game runs
 * Then the Drifter is in the world: a stage creates bodies as a behaviour does
 */
TEST_F(GameFlowSendingLevelFixture, BringsInTheBodiesAStageCreates) {
  startGameWith({FIRST_PLAYER});

  runTicks(1);

  EXPECT_EQ(enemiesInPlay(), 1U);
}

/**
 * Given a game that plays a level of one stage, which ends at once
 * When the level is cleared
 * Then the game is won, and its player is told so
 */
TEST_F(GameFlowOneStageFixture, IsWonOnceTheLevelIsCleared) {
  startGameWith({FIRST_PLAYER});

  runTicks(2);

  const std::vector<GameEnded> ends = endsSentTo(FIRST_PLAYER);
  ASSERT_FALSE(ends.empty());
  EXPECT_EQ(ends.front().outcome, WireGameOutcome::Victory);
}

/**
 * Given a game that plays a level whose script fails
 * When the stage that fails runs
 * Then the game is lost, and its player is told so
 */
TEST_F(GameFlowBrokenLevelFixture, IsLostOnceTheLevelFailed) {
  startGameWith({FIRST_PLAYER});

  runTicks(2);

  const std::vector<GameEnded> ends = endsSentTo(FIRST_PLAYER);
  ASSERT_FALSE(ends.empty());
  EXPECT_EQ(ends.front().outcome, WireGameOutcome::Defeat);
}
