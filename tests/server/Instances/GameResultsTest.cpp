#include <gtest/gtest.h>
#include "GameEnded.hpp"
#include "GameInstanceFixtures.hpp"
#include "GameInstanceLivesFixture.hpp"
#include "GameInstanceTestConstants.hpp"
#include "GameResult.hpp"
#include "GameResultsFixtures.hpp"
#include "GameRuleConstants.hpp"
#include "GameState.hpp"
#include "LeaderboardSamples.hpp"
#include "MessagePayload.hpp"
#include "MessageType.hpp"
#include "Replication/Announcements/Milestones/GameEndAnnouncerTestConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerConstants.hpp"

namespace rtype::server::testing {

/**
 * Given a real game of two players with their final scores
 * When both lose their last life and the tick that ends the game runs
 * Then one result is recorded in that very tick, and it holds the scores
 * GameEnded tells them, under the difficulty of the game
 */
TEST_F(GameInstanceLivesFixture, RecordsTheScoresGameEndedAnnounces) {
  startGame();
  giveTheFinalScores();

  loseEveryLife();

  ASSERT_EQ(results().results().size(), 1U);
  const GameResult result = results().results().front();
  EXPECT_EQ(result.difficulty, rtype::identifiers::DEFAULT_DIFFICULTY);
  EXPECT_EQ(result.playerCount, 2U);
  ASSERT_EQ(result.players.size(), 2U);
  EXPECT_EQ(result.players.at(0).score, FIRST_PLAYER_FINAL_SCORE);
  EXPECT_EQ(result.players.at(1).score, SECOND_PLAYER_FINAL_SCORE);
  const auto ended = onlyMessageIn<network::GameEnded>(
      ofType(network::MessageType::GameEnded, sentTo(FIRST_PLAYER)).front());
  EXPECT_EQ(ended.players.at(0).score, result.players.at(0).score);
  EXPECT_EQ(ended.players.at(1).score, result.players.at(1).score);
}

/**
 * Given a real game in which the first player is hit once and leaves
 * When the second player leaves too, which ends the game
 * Then both have a row, one death for the first and none for the second
 */
TEST_F(GameInstanceLivesFixture, CountsTheHitsOfAPlayerWhoLeft) {
  startGame();
  hitTheShipOf(FIRST_PLAYER);

  leave(FIRST_PLAYER);
  tick();
  leave(SECOND_PLAYER);
  tick();
  events().dispatch();

  ASSERT_EQ(game().state(), GameState::Finished);
  ASSERT_EQ(results().results().size(), 1U);
  const GameResult result = results().results().front();
  ASSERT_EQ(result.players.size(), 2U);
  EXPECT_EQ(result.players.at(0).deaths, 1U);
  EXPECT_EQ(result.players.at(1).deaths, 0U);
}

/**
 * Given a game in progress
 * When it is ended without a result for the players
 * Then its result is recorded all the same
 */
TEST_F(GameInstanceTest, RecordsAGameEndedWithoutAResultForThePlayers) {
  startGameWithOnePlayer();

  game().finish();
  tick();
  events().dispatch();

  EXPECT_EQ(results().results().size(), 1U);
}

/**
 * Given a seated player
 * When a datagram holding a LeaderboardRequest arrives, then the answer is
 * ready Then the archive got the request and the player the answer: the handler
 * is registered, and the request goes through the checks of a real datagram
 */
TEST_F(GameInstanceTest, AnswersALeaderboardRequestThroughARealDatagram) {
  join(FIRST_PLAYER);
  queue(leaderboardRequestFrom(FIRST_PLAYER));

  tick();
  ASSERT_EQ(results().asks().size(), 1U);
  results().answerEveryAsk(network::encodeMessage(emptyLeaderboardAnswer()));
  tick();

  EXPECT_EQ(
      ofType(network::MessageType::LeaderboardAnswer, sentTo(FIRST_PLAYER))
          .size(),
      1U);
}

/**
 * Given a datagram that holds a Disconnection, then a LeaderboardRequest
 * When a tick reads it
 * Then the seat is already free when the second message is read: no ask, and
 * nothing goes wrong
 */
TEST_F(GameInstanceTest, IgnoresARequestRightAfterTheDisconnectionOfItsSender) {
  join(FIRST_PLAYER);
  queue(disconnectionThenLeaderboardRequestFrom(FIRST_PLAYER));

  tick();

  EXPECT_TRUE(results().asks().empty());
}

}  // namespace rtype::server::testing
