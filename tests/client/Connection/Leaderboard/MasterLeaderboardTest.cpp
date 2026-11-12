#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>
#include "GlobalLeaderboardQuery.hpp"
#include "LeaderboardAnswer.hpp"
#include "LeaderboardPhase.hpp"
#include "LeaderboardRequest.hpp"
#include "MasterLeaderboardFixture.hpp"
#include "MasterLeaderboardTestConstants.hpp"

namespace rtype::client::testing {

/**
 * Given a leaderboard of the master
 * When the screen asks for every difficulty and every number of pilots
 * Then the master is asked for neither, so it reads every game
 */
TEST_F(MasterLeaderboardFixture, AsksTheMasterForEveryGameWhenAllIsPicked) {
  leaderboard().ask(
      network::LeaderboardRequest{.playerCount = 0, .difficulty = ""});

  EXPECT_EQ(reads().queries(),
            (std::vector<network::GlobalLeaderboardQuery>{
                network::GlobalLeaderboardQuery{.difficulty = std::nullopt,
                                                .playerCount = std::nullopt,
                                                .player = std::nullopt,
                                                .limit = GLOBAL_ROWS_ASKED}}));
}

/**
 * Given a leaderboard of the master that named NOVA as its player
 * When the screen asks for the leaderboard of two pilots in normal
 * Then the master is asked for that filter, its first ten rows and the row of
 * NOVA
 */
TEST_F(MasterLeaderboardFixture, AsksTheMasterForTheFilterAndThePlayer) {
  leaderboard().namePlayer(std::string{GLOBAL_PLAYER});

  leaderboard().ask(askedRequest());

  EXPECT_EQ(reads().queries(),
            (std::vector<network::GlobalLeaderboardQuery>{
                network::GlobalLeaderboardQuery{
                    .difficulty = std::string{GLOBAL_ASKED_DIFFICULTY},
                    .playerCount = GLOBAL_ASKED_PILOTS,
                    .player = std::string{GLOBAL_PLAYER},
                    .limit = GLOBAL_ROWS_ASKED}}));
}

/**
 * Given a leaderboard named an empty nickname, which the protocol refuses
 * When the screen asks for a leaderboard
 * Then the master is asked for no row of a player
 */
TEST_F(MasterLeaderboardFixture, ANicknameTheProtocolRefusesNamesNoPlayer) {
  leaderboard().namePlayer(std::string{EMPTY_NICKNAME});

  leaderboard().ask(askedRequest());

  ASSERT_EQ(reads().queries().size(), 1U);
  EXPECT_EQ(reads().queries().front().player, std::nullopt);
}

/**
 * Given an ask the master has not answered yet
 * When a frame goes by
 * Then the leaderboard is still asking, with no row
 */
TEST_F(MasterLeaderboardFixture, AsksUntilTheMasterAnswers) {
  leaderboard().ask(askedRequest());

  leaderboard().update();

  EXPECT_EQ(leaderboard().page().phase, LeaderboardPhase::Asking);
  EXPECT_TRUE(leaderboard().page().rows.empty());
}

/**
 * Given an ask the master answered with KIRA first and NOVA, the player,
 * second
 * When a frame goes by
 * Then the page shows both rows, best first, and NOVA as the row of the player
 */
TEST_F(MasterLeaderboardFixture, ShowsTheRowsTheMasterAnswered) {
  leaderboard().ask(askedRequest());
  reads().answer(1, masterAnswer());

  leaderboard().update();

  EXPECT_EQ(leaderboard().page().phase, LeaderboardPhase::Answered);
  EXPECT_EQ(leaderboard().page().rows,
            (std::vector<network::LeaderboardRow>{kiraShown(), novaShown()}));
  EXPECT_EQ(leaderboard().page().ownRow, novaShown());
}

/**
 * Given an ask the master did not answer, or answered what cannot be read
 * When a frame goes by
 * Then the page says the master is out of reach
 */
TEST_F(MasterLeaderboardFixture, AMasterThatDidNotAnswerIsOutOfReach) {
  leaderboard().ask(askedRequest());
  reads().answer(1, std::nullopt);

  leaderboard().update();

  EXPECT_EQ(leaderboard().page().phase, LeaderboardPhase::OutOfReach);
}

/**
 * Given two asks, the master answering the first one only
 * When a frame goes by
 * Then the page still waits for the second: the rows of an older filter
 * never show
 */
TEST_F(MasterLeaderboardFixture, TheAnswerToAnOlderAskIsIgnored) {
  leaderboard().ask(askedRequest());
  leaderboard().ask(askedRequest());
  reads().answer(1, masterAnswer());

  leaderboard().update();

  EXPECT_EQ(leaderboard().page().phase, LeaderboardPhase::Asking);
}

/**
 * Given an ask the screen stopped, then the answer to it
 * When a frame goes by
 * Then the page is left as it was: a closed screen takes no answer
 */
TEST_F(MasterLeaderboardFixture, AnAnswerAfterTheScreenStoppedIsIgnored) {
  leaderboard().ask(askedRequest());
  leaderboard().stopAsking();
  reads().answer(1, masterAnswer());

  leaderboard().update();

  EXPECT_EQ(leaderboard().page().phase, LeaderboardPhase::Asking);
}

}  // namespace rtype::client::testing
