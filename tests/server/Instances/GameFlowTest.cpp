#include <gtest/gtest.h>
#include <cstddef>
#include "ConnectedPlayer.hpp"
#include "EntityIdConstants.hpp"
#include "GameFlowFixture.hpp"
#include "GameInstanceConstants.hpp"
#include "GameInstanceTestConstants.hpp"
#include "GameState.hpp"
#include "PlayerRoster.hpp"
#include "SentDatagrams.hpp"
#include "Spawning/BydoSpawnerTestConstants.hpp"

using rtype::identifiers::NO_ENTITY_ID;
using rtype::server::ConnectedPlayer;
using rtype::server::FINISHED_STATE_TICKS;
using rtype::server::GameState;
using rtype::server::testing::FIRST_PLAYER;
using rtype::server::testing::FOURTH_PLAYER;
using rtype::server::testing::GameFlowFixture;
using rtype::server::testing::playerEndpoint;
using rtype::server::testing::SECOND_PLAYER;
using rtype::server::testing::SOME_TICKS;
using rtype::server::testing::THIRD_PLAYER;
using rtype::server::testing::TICKS_TO_THE_FIRST_BYDO;

/**
 * Given a game that was just built
 * When nothing happened yet
 * Then it waits for players, at tick 0
 */
TEST_F(GameFlowFixture, StartsWaitingForPlayers) {
  EXPECT_EQ(flow().state(), GameState::WaitingForPlayers);
  EXPECT_EQ(flow().tick(), 0);
}

/**
 * Given a game with no connected player
 * When ticks run
 * Then it keeps waiting for players
 */
TEST_F(GameFlowFixture, DoesNotStartWithoutPlayers) {
  runTicks(SOME_TICKS);

  EXPECT_EQ(flow().state(), GameState::WaitingForPlayers);
}

/**
 * Given one connected player who is not ready
 * When ticks run
 * Then the game keeps waiting
 */
TEST_F(GameFlowFixture, WaitsForAPlayerWhoIsNotReady) {
  join(FIRST_PLAYER);

  runTicks(SOME_TICKS);

  EXPECT_EQ(flow().state(), GameState::WaitingForPlayers);
}

/**
 * Given one connected player
 * When the player is ready and a tick runs
 * Then the game is in progress
 */
TEST_F(GameFlowFixture, StartsWhenTheOnlyPlayerIsReady) {
  join(FIRST_PLAYER).ready = true;

  tick();

  EXPECT_EQ(flow().state(), GameState::InProgress);
}

/**
 * Given two connected players
 * When only the first is ready
 * Then the game keeps waiting
 */
TEST_F(GameFlowFixture, WaitsWhileOnePlayerIsNotReady) {
  join(FIRST_PLAYER).ready = true;
  join(SECOND_PLAYER);

  tick();

  EXPECT_EQ(flow().state(), GameState::WaitingForPlayers);
}

/**
 * Given four connected players, the most a game holds
 * When every one of them is ready
 * Then the game is in progress
 */
TEST_F(GameFlowFixture, StartsWhenFourPlayersAreReady) {
  startGameWith({FIRST_PLAYER, SECOND_PLAYER, THIRD_PLAYER, FOURTH_PLAYER});

  EXPECT_EQ(flow().state(), GameState::InProgress);
}

/**
 * Given a game that waits for players
 * When ticks run
 * Then the tick stays at 0
 */
TEST_F(GameFlowFixture, DoesNotCountTicksWhileWaiting) {
  join(FIRST_PLAYER);

  runTicks(SOME_TICKS);

  EXPECT_EQ(flow().tick(), 0);
}

/**
 * Given a game that just started
 * When SOME_TICKS ticks run
 * Then the tick is SOME_TICKS
 */
TEST_F(GameFlowFixture, CountsTicksWhileInProgress) {
  startGameWithOnePlayer();

  runTicks(SOME_TICKS);

  EXPECT_EQ(flow().tick(), SOME_TICKS);
}

/**
 * Given a game in progress
 * When it is finished
 * Then its state is Finished
 */
TEST_F(GameFlowFixture, FinishEndsTheGameInProgress) {
  startGameWithOnePlayer();

  flow().finish();

  EXPECT_EQ(flow().state(), GameState::Finished);
}

/**
 * Given a game that waits for players
 * When it is finished
 * Then it keeps waiting
 */
TEST_F(GameFlowFixture, FinishDoesNothingWhileWaiting) {
  flow().finish();

  EXPECT_EQ(flow().state(), GameState::WaitingForPlayers);
}

/**
 * Given a finished game
 * When one tick less than FINISHED_STATE_TICKS runs
 * Then it is still finished
 */
TEST_F(GameFlowFixture, StaysFinishedBeforeTheFinishedDurationElapsed) {
  startGameWithOnePlayer();
  flow().finish();

  runTicks(FINISHED_STATE_TICKS - 1);

  EXPECT_EQ(flow().state(), GameState::Finished);
}

/**
 * Given a finished game
 * When FINISHED_STATE_TICKS ticks run
 * Then it waits for players again
 */
TEST_F(GameFlowFixture, WaitsForPlayersAgainOnceTheFinishedDurationElapsed) {
  startGameWithOnePlayer();
  flow().finish();

  runTicks(FINISHED_STATE_TICKS);

  EXPECT_EQ(flow().state(), GameState::WaitingForPlayers);
}

/**
 * Given a game in progress whose only player was removed from the roster
 * When a tick runs
 * Then the game is finished in that tick
 */
TEST_F(GameFlowFixture, FinishesTheGameInProgressNoPlayerIsLeftIn) {
  startGameWithOnePlayer();
  ASSERT_TRUE(roster().remove(playerEndpoint(FIRST_PLAYER)).has_value());

  tick();

  EXPECT_EQ(flow().state(), GameState::Finished);
}

/**
 * Given a game in progress whose only player has no life left
 * When a tick runs
 * Then the game is finished in that tick
 */
TEST_F(GameFlowFixture, FinishesTheGameInProgressNoPlayerHasALife) {
  ConnectedPlayer& player = join(FIRST_PLAYER);
  player.ready = true;
  tick();
  ASSERT_EQ(flow().state(), GameState::InProgress);
  player.lives = 0;

  tick();

  EXPECT_EQ(flow().state(), GameState::Finished);
}

/**
 * Given a game in progress with two players, one of whom has no life left
 * When a tick runs
 * Then the game goes on for the other
 */
TEST_F(GameFlowFixture, PlaysOnWhileAPlayerHasALife) {
  startGameWith({FIRST_PLAYER, SECOND_PLAYER});
  roster().find(playerEndpoint(FIRST_PLAYER))->lives = 0;

  tick();

  EXPECT_EQ(flow().state(), GameState::InProgress);
}

/**
 * Given a game that was played by two players and finished
 * When it waits for players again
 * Then the world holds one fresh ship per player, not the old ones as well,
 * and the tick is back at 0
 */
TEST_F(GameFlowFixture, ResetsTheWorldBetweenTwoGames) {
  startGameWith({FIRST_PLAYER, SECOND_PLAYER});
  runTicks(SOME_TICKS);
  flow().finish();

  runTicks(FINISHED_STATE_TICKS);

  EXPECT_EQ(flow().world().entityCount(), 2);
  EXPECT_EQ(flow().tick(), 0);
}

/**
 * Given a game that was played and finished
 * When it waits for players again
 * Then every player is connected, not ready, and has a ship
 */
TEST_F(GameFlowFixture, KeepsThePlayersConnectedButNotReadyForTheNextGame) {
  startGameWithOnePlayer();
  flow().finish();

  runTicks(FINISHED_STATE_TICKS);

  ASSERT_EQ(roster().connected().size(), 1);
  const ConnectedPlayer* const player =
      roster().find(playerEndpoint(FIRST_PLAYER));
  ASSERT_NE(player, nullptr);
  EXPECT_FALSE(player->ready);
  EXPECT_NE(player->shipEntityId, NO_ENTITY_ID);
}

/**
 * Given a game that waits for players again after a game
 * When ticks run without any player being ready
 * Then the game keeps waiting
 */
TEST_F(GameFlowFixture, NeedsEveryPlayerReadyAgainForTheNextGame) {
  startGameWithOnePlayer();
  flow().finish();
  runTicks(FINISHED_STATE_TICKS);

  runTicks(SOME_TICKS);

  EXPECT_EQ(flow().state(), GameState::WaitingForPlayers);
}

/**
 * Given a game that waits for a player who is not ready
 * When as many ticks pass as the first pause of a game
 * Then the world holds the ship only: no Bydo appears before the game starts
 */
TEST_F(GameFlowFixture, NoBydoAppearsWhileWaitingForPlayers) {
  join(FIRST_PLAYER);

  runTicks(TICKS_TO_THE_FIRST_BYDO);

  EXPECT_EQ(flow().world().entityCount(), 1U);
}

/**
 * Given a game just started with one player
 * When one tick less than the first pause passes, then one more
 * Then no Bydo is in the world, and then one is, next to the ship
 */
TEST_F(GameFlowFixture, TheFirstBydoAppearsOnceTheFirstPauseIsOver) {
  startGameWithOnePlayer();

  runTicks(TICKS_TO_THE_FIRST_BYDO - 1);
  EXPECT_EQ(flow().world().entityCount(), 1U);
  tick();

  EXPECT_EQ(flow().world().entityCount(), 2U);
}

/**
 * Given a finished game, which had run half its first pause
 * When it waits for players again, starts again, and runs one tick less than
 * the first pause
 * Then no Bydo is in the new world: its waves start over with the first pause
 */
TEST_F(GameFlowFixture, ANewGameStartsTheWavesOver) {
  startGameWithOnePlayer();
  runTicks(TICKS_TO_THE_FIRST_BYDO / 2);
  flow().finish();
  runTicks(FINISHED_STATE_TICKS);
  ASSERT_EQ(flow().state(), GameState::WaitingForPlayers);
  roster().find(playerEndpoint(FIRST_PLAYER))->ready = true;
  tick();
  ASSERT_EQ(flow().state(), GameState::InProgress);

  runTicks(TICKS_TO_THE_FIRST_BYDO - 1);

  EXPECT_EQ(flow().world().entityCount(), 1U);
}

/**
 * Given a finished game
 * When ticks run for twice the first pause
 * Then no Bydo appears
 */
TEST_F(GameFlowFixture, NoBydoAppearsWhileFinished) {
  startGameWithOnePlayer();
  flow().finish();
  const std::size_t entitiesBefore = flow().world().entityCount();

  runTicks(2 * TICKS_TO_THE_FIRST_BYDO);

  EXPECT_EQ(flow().world().entityCount(), entitiesBefore);
}
