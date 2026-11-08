#include <gtest/gtest.h>
#include <string>
#include <thread>
#include "GameResult.hpp"
#include "HeartbeatJobFixture.hpp"
#include "HeartbeatJobTestConstants.hpp"
#include "ResultsFileFixtures.hpp"

using rtype::server::GameResult;
using rtype::server::testing::FAST_TIMES;
using rtype::server::testing::FIRST_PILOT_NAME;
using rtype::server::testing::GAME_END;
using rtype::server::testing::HeartbeatJobFixture;
using rtype::server::testing::LATER_GAME_END;
using rtype::server::testing::MASTER_DOWN_TIME;
using rtype::server::testing::NO_PILOT_AT_THE_START;
using rtype::server::testing::NO_ROW;
using rtype::server::testing::ONE_ROW;
using rtype::server::testing::SECOND_PILOT_NAME;
using rtype::server::testing::vanillaGameOf;

/**
 * Given a vanilla game of one pilot in the file of results
 * When the job starts and its first heartbeat reaches the master
 * Then the master keeps the row of that pilot, and the file says it was sent
 */
TEST_F(HeartbeatJobFixture, SendsAWaitingVanillaGameAfterTheHeartbeat) {
  const std::string game =
      saveResult(vanillaGameOf(FIRST_PILOT_NAME, GAME_END));

  startJob(FAST_TIMES);

  EXPECT_TRUE(masterSoonKeeps(game, ONE_ROW));
  EXPECT_TRUE(soonAnswered(game).sent);
}

/**
 * Given a job registered with the master
 * When the master stops, a vanilla game ends while it is down, then the master
 * comes back
 * Then the game waits unsent while the master is down, and reaches it once it
 * is back
 */
TEST_F(HeartbeatJobFixture, SendsTheResultsThatWaitedWhileTheMasterWasDown) {
  startJob(FAST_TIMES);
  ASSERT_TRUE(
      soonAnswered(saveResult(vanillaGameOf(FIRST_PILOT_NAME, GAME_END))).sent);

  master().stop();
  const std::string game =
      saveResult(vanillaGameOf(SECOND_PILOT_NAME, LATER_GAME_END));
  std::this_thread::sleep_for(MASTER_DOWN_TIME);
  ASSERT_FALSE(answersOf(game).sent);
  master().restart();

  EXPECT_TRUE(masterSoonKeeps(game, ONE_ROW));
  EXPECT_TRUE(soonAnswered(game).sent);
}

/**
 * Given a game that is not vanilla, then a vanilla game, in the file of
 * results
 * When the job sends the waiting results
 * Then the vanilla game reaches the master, and the other one never leaves
 * the server
 */
TEST_F(HeartbeatJobFixture, NeverSendsAGameThatIsNotVanilla) {
  GameResult modded = vanillaGameOf(FIRST_PILOT_NAME, GAME_END);
  modded.vanilla = false;
  const std::string moddedGame = saveResult(modded);
  const std::string vanillaGame =
      saveResult(vanillaGameOf(SECOND_PILOT_NAME, LATER_GAME_END));

  startJob(FAST_TIMES);

  ASSERT_TRUE(soonAnswered(vanillaGame).sent);
  EXPECT_EQ(master().keptPlayersOf(moddedGame), NO_ROW);
  EXPECT_FALSE(answersOf(moddedGame).sent);
}

/**
 * Given a vanilla game the master refuses for good, a game of no pilot at its
 * start, then a vanilla game it keeps, in the file of results
 * When the job sends the waiting results
 * Then the first is marked refused and never sent, and the second reaches the
 * master all the same
 */
TEST_F(HeartbeatJobFixture, MarksAGameRefusedForGoodAndSendsTheNextOne) {
  GameResult refused = vanillaGameOf(FIRST_PILOT_NAME, GAME_END);
  refused.playerCount = NO_PILOT_AT_THE_START;
  const std::string refusedGame = saveResult(refused);
  const std::string keptGame =
      saveResult(vanillaGameOf(SECOND_PILOT_NAME, LATER_GAME_END));

  startJob(FAST_TIMES);

  EXPECT_TRUE(soonAnswered(refusedGame).refused);
  EXPECT_TRUE(masterSoonKeeps(keptGame, ONE_ROW));
  EXPECT_EQ(master().keptPlayersOf(refusedGame), NO_ROW);
  EXPECT_FALSE(answersOf(refusedGame).sent);
}
