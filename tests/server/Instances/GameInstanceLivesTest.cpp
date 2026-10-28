#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EntityDestruction.hpp"
#include "EntityIdConstants.hpp"
#include "GameEnded.hpp"
#include "GameEvent.hpp"
#include "GameInstanceConstants.hpp"
#include "GameInstanceFixtures.hpp"
#include "GameInstanceLivesFixture.hpp"
#include "GameInstanceTestConstants.hpp"
#include "GameState.hpp"
#include "LifeKeeperTestConstants.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "Replication/GameEndAnnouncerTestConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerConstants.hpp"

namespace rtype::server::testing {

/**
 * Given a game in progress
 * When the ship of the first player touches a Bydo
 * Then the first player has one life less
 */
TEST_F(GameInstanceLivesFixture, TakesALifeWhenAShipTouchesABydo) {
  startGame();

  hitTheShipOf(FIRST_PLAYER);

  EXPECT_EQ(playerAt(FIRST_PLAYER).lives, THREE_LIVES - 1);
}

/**
 * Given a game in progress with two players
 * When the ship of the first player touches a Bydo
 * Then the world holds only the ship of the second player: the first ship
 * and the Bydo are gone
 */
TEST_F(GameInstanceLivesFixture, LeavesOnlyTheOtherShipInTheWorld) {
  startGame();

  hitTheShipOf(FIRST_PLAYER);

  EXPECT_EQ(game().world().entityCount(), 1U);
}

/**
 * Given a game in progress whose events go through the broadcaster of the
 * server
 * When the ship of the first player touches a Bydo
 * Then every player is sent a PlayerHit naming the first player
 */
TEST_F(GameInstanceLivesFixture, TellsEveryPlayerWhoWasHit) {
  startGame();

  hitTheShipOf(FIRST_PLAYER);

  for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
    const std::vector<network::OutgoingDatagram> received =
        ofType(network::MessageType::GameEvent,
               testing::sentTo(sentDatagrams(), playerEndpoint(player)));
    ASSERT_EQ(received.size(), 1U);
    const auto event = onlyMessageIn<network::GameEvent>(received.front());
    EXPECT_EQ(event.kind, network::GameEventKind::PlayerHit);
    EXPECT_EQ(event.playerNumber, playerAt(FIRST_PLAYER).playerNumber);
  }
}

/**
 * Given a game in progress whose first ship touched a Bydo
 * When the ticks of one second run
 * Then the first player has a ship again
 */
TEST_F(GameInstanceLivesFixture, BringsTheShipBackAfterOneSecond) {
  startGame();
  hitTheShipOf(FIRST_PLAYER);

  runTicksDispatching(ONE_SECOND_IN_TICKS);

  EXPECT_NE(shipIdOf(FIRST_PLAYER), identifiers::NO_ENTITY_ID);
}

/**
 * Given a game whose first ship touched a Bydo, finished
 * When the game waits for players again
 * Then the first player has three lives again
 */
TEST_F(GameInstanceLivesFixture, GivesThreeLivesBackWithANewWorld) {
  startGame();
  hitTheShipOf(FIRST_PLAYER);
  game().finish();

  runTicksDispatching(FINISHED_STATE_TICKS);

  ASSERT_EQ(game().state(), GameState::WaitingForPlayers);
  EXPECT_EQ(playerAt(FIRST_PLAYER).lives, THREE_LIVES);
}

/**
 * Given a game finished while the first player waited for its ship
 * When the next game runs for one second and a world state goes out
 * Then it lists the first player as vulnerable: the wait of the previous game
 * brought no second ship
 */
TEST_F(GameInstanceLivesFixture, ForgetsTheWaitOfThePreviousGame) {
  startGame();
  hitTheShipOf(FIRST_PLAYER);
  game().finish();
  runTicksDispatching(FINISHED_STATE_TICKS);
  startTheNextGame();

  runTicksDispatching(ONE_SECOND_IN_TICKS + TICKS_PER_WORLD_STATE);

  EXPECT_FALSE(newestStateOf(FIRST_PLAYER).invulnerable);
}

/**
 * Given a game in progress whose first ship touched a Bydo
 * When the next world state goes out
 * Then it lists the first player with one life less
 */
TEST_F(GameInstanceLivesFixture, ListsTheLivesLeftInTheWorldState) {
  startGame();
  hitTheShipOf(FIRST_PLAYER);

  runTicksDispatching(TICKS_PER_WORLD_STATE);

  EXPECT_EQ(newestStateOf(FIRST_PLAYER).lives, THREE_LIVES - 1);
}

/**
 * Given a game in progress whose first ship came back after a hit
 * When the next world state goes out
 * Then it lists the first player as invulnerable
 */
TEST_F(GameInstanceLivesFixture, ListsTheShipThatCameBackAsInvulnerable) {
  startGame();
  hitTheShipOf(FIRST_PLAYER);

  runTicksDispatching(ONE_SECOND_IN_TICKS + TICKS_PER_WORLD_STATE);

  EXPECT_TRUE(newestStateOf(FIRST_PLAYER).invulnerable);
}

/**
 * Given a game in progress
 * When a missile of an enemy hits the ship of the first player
 * Then the first player has one life less
 */
TEST_F(GameInstanceLivesFixture, TakesALifeWhenAnEnemyMissileHitsAShip) {
  startGame();

  shootTheShipOf(FIRST_PLAYER);

  EXPECT_EQ(playerAt(FIRST_PLAYER).lives, THREE_LIVES - 1);
}

/**
 * Given a game in progress whose destructions are recorded
 * When a missile of an enemy hits the ship of the first player
 * Then the ship is announced killed and the missile removed, once each: the
 * damage rules remove the missile before the keeper of the lives takes the ship
 */
TEST_F(GameInstanceLivesFixture, DestroysTheShipAndRemovesTheMissileThatHitIt) {
  recordDestructions();
  startGame();
  const std::uint32_t ship = shipIdOf(FIRST_PLAYER);

  const std::uint32_t missile = shootTheShipOf(FIRST_PLAYER);

  EXPECT_TRUE(wasDestroyed(ship, game::DestructionCause::Killed));
  EXPECT_TRUE(wasDestroyed(missile, game::DestructionCause::Removed));
}

/**
 * Given a game in progress whose events go through the broadcaster of the
 * server
 * When a missile of an enemy hits the ship of the first player
 * Then every player is sent a PlayerHit naming the first player
 */
TEST_F(GameInstanceLivesFixture, TellsEveryPlayerWhoWasShot) {
  startGame();

  shootTheShipOf(FIRST_PLAYER);

  for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
    const std::vector<network::OutgoingDatagram> received =
        ofType(network::MessageType::GameEvent,
               testing::sentTo(sentDatagrams(), playerEndpoint(player)));
    ASSERT_EQ(received.size(), 1U);
    const auto event = onlyMessageIn<network::GameEvent>(received.front());
    EXPECT_EQ(event.kind, network::GameEventKind::PlayerHit);
    EXPECT_EQ(event.playerNumber, playerAt(FIRST_PLAYER).playerNumber);
  }
}

/**
 * Given a game in progress whose first ship came back after a missile of an
 * enemy hit it
 * When a second missile hits the new ship
 * Then the first player still has only one life less: the new ship cannot be
 * hit yet
 */
TEST_F(GameInstanceLivesFixture, SparesTheShipThatCameBackFromAMissile) {
  startGame();
  shootTheShipOf(FIRST_PLAYER);
  runTicksDispatching(ONE_SECOND_IN_TICKS);

  shootTheShipOf(FIRST_PLAYER);

  EXPECT_EQ(playerAt(FIRST_PLAYER).lives, THREE_LIVES - 1);
}

/**
 * Given a game in progress whose first player lost its last life
 * When the second player loses its last life, then the next tick runs
 * Then the game is finished: no player has a life left
 */
TEST_F(GameInstanceLivesFixture, EndsTheGameOnceNoPlayerHasALifeLeft) {
  startGame();
  takeTheLastLifeOf(FIRST_PLAYER);

  takeTheLastLifeOf(SECOND_PLAYER);
  runTicksDispatching(1);

  EXPECT_EQ(game().state(), GameState::Finished);
}

/**
 * Given a game in progress
 * When the first player loses its last life, then a few ticks run
 * Then the game goes on: the second player has a life left
 */
TEST_F(GameInstanceLivesFixture, GoesOnWhileAPlayerHasALifeLeft) {
  startGame();

  takeTheLastLifeOf(FIRST_PLAYER);
  runTicksDispatching(SOME_TICKS);

  EXPECT_EQ(game().state(), GameState::InProgress);
}

/**
 * Given a game in progress whose first player lost its last life
 * When the ship of the second player touches a Bydo, which leaves that player
 * lives but no ship until it comes back, then the next tick runs
 * Then the game goes on: a player who waits for its ship has a life left
 */
TEST_F(GameInstanceLivesFixture, GoesOnWhileAPlayerWaitsForItsShip) {
  startGame();
  takeTheLastLifeOf(FIRST_PLAYER);

  hitTheShipOf(SECOND_PLAYER);
  runTicksDispatching(1);

  EXPECT_EQ(game().state(), GameState::InProgress);
}

/**
 * Given a game in progress whose two players scored
 * When both lose their last life
 * Then each player is sent one GameEnded listing both, in order of number,
 * with the score each ended the game with
 */
TEST_F(GameInstanceLivesFixture, SendsEveryPlayerTheScoresAtTheEnd) {
  startGame();
  giveTheFinalScores();

  loseEveryLife();

  const network::GameEnded expected{
      .players = {network::PlayerScore{.playerNumber = FIRST_JOINED_NUMBER,
                                       .score = FIRST_PLAYER_FINAL_SCORE},
                  network::PlayerScore{.playerNumber = SECOND_JOINED_NUMBER,
                                       .score = SECOND_PLAYER_FINAL_SCORE}}};
  for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
    const std::vector<network::OutgoingDatagram> ends =
        ofType(network::MessageType::GameEnded, sentTo(player));
    ASSERT_EQ(ends.size(), 1U);
    EXPECT_EQ(onlyMessageIn<network::GameEnded>(ends.front()), expected);
  }
}

/**
 * Given a game whose two players lost their last life, which sent them the
 * end
 * When the 500 ms the protocol sets between two GameEnded run
 * Then the first player was sent the end once more, so a lost one is
 * corrected
 */
TEST_F(GameInstanceLivesFixture, RepeatsTheEndWhileTheGameIsFinished) {
  startGame();
  loseEveryLife();

  runTicksDispatching(PROTOCOL_GAME_ENDED_TICKS);

  EXPECT_EQ(
      ofType(network::MessageType::GameEnded, sentTo(FIRST_PLAYER)).size(), 2U);
}

/**
 * Given a game whose two players lost their last life, waiting for players
 * again once its finished state is over
 * When both players say ready again, then a few ticks of the new game run
 * Then the new game is in progress: no restart of the server is needed
 */
TEST_F(GameInstanceLivesFixture, PlaysANewGameAfterTheEnd) {
  startGame();
  loseEveryLife();
  runTicksDispatching(FINISHED_STATE_TICKS);

  startTheNextGame();
  runTicksDispatching(SOME_TICKS);

  EXPECT_EQ(game().state(), GameState::InProgress);
}

}  // namespace rtype::server::testing
