#include <gtest/gtest.h>
#include <string>
#include "ConnectedPlayer.hpp"
#include "EnemyKilled.hpp"
#include "EventBus.hpp"
#include "PlayerRoster.hpp"
#include "ProtocolConstants.hpp"
#include "ScoreKeeper.hpp"
#include "ScoreKeeperFixture.hpp"
#include "ScoreKeeperTestConstants.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"

namespace rtype::server::testing {

/**
 * Given two connected players with no score
 * When the second player kills an enemy worth points
 * Then the second player has those points and the first still has none
 */
TEST_F(ScoreKeeperFixture, CreditsOnlyThePlayerWhoFired) {
  kill(SECOND_NUMBER, DRIFTER_WORTH);

  EXPECT_EQ(secondScore(), DRIFTER_WORTH);
  EXPECT_EQ(firstScore(), 0U);
}

/**
 * Given a player who already killed a Drifter
 * When the player kills a Weaver
 * Then the score is the sum of both
 */
TEST_F(ScoreKeeperFixture, AddsEachKillToTheScore) {
  kill(FIRST_NUMBER, DRIFTER_WORTH);
  kill(FIRST_NUMBER, WEAVER_WORTH);

  EXPECT_EQ(firstScore(), DRIFTER_WORTH + WEAVER_WORTH);
}

/**
 * Given two connected players
 * When a kill names a number nobody holds, 0, or a number above the maximum
 * Then no score changes
 */
TEST_F(ScoreKeeperFixture, IgnoresAKillOfANumberNobodyHolds) {
  kill(network::FIRST_PLAYER_NUMBER + 2, DRIFTER_WORTH);
  kill(network::NO_PLAYER_NUMBER, DRIFTER_WORTH);
  kill(network::MAXIMUM_PLAYERS + 1, DRIFTER_WORTH);

  EXPECT_EQ(firstScore(), 0U);
  EXPECT_EQ(secondScore(), 0U);
}

/**
 * Given a player whose score is 10 below the largest value
 * When the player kills an enemy worth more than 10
 * Then the score stops at the largest value instead of wrapping around
 */
TEST_F(ScoreKeeperFixture, StopsTheScoreAtItsLargestValue) {
  giveFirstPlayerScore(LARGEST_SCORE - TEN_POINTS);

  kill(FIRST_NUMBER, DRIFTER_WORTH);

  EXPECT_EQ(firstScore(), LARGEST_SCORE);
}

/**
 * Given two connected players
 * When the second destroys a boss worth 3 000 points, the first having dealt
 * it three damage and the second one
 * Then the first has 2 250 points and the second 750
 */
TEST_F(ScoreKeeperFixture, ABossSharesItsPointsByTheDamageOfEachPilot) {
  killBoss(SECOND_NUMBER, BOSS_WORTH, THREE_THEN_ONE);

  EXPECT_EQ(firstScore(), THREE_QUARTERS_OF_THE_BOSS);
  EXPECT_EQ(secondScore(), A_QUARTER_OF_THE_BOSS);
}

/**
 * Given two connected players
 * When the first destroys a boss worth 1 000 points, having dealt it one
 * damage and the second two
 * Then the first has 334 points, its third and the point the division left,
 * and the second 666
 */
TEST_F(ScoreKeeperFixture, TheRestOfTheDivisionGoesToThePilotWhoDestroyedIt) {
  killBoss(FIRST_NUMBER, THOUSAND_POINTS, ONE_THEN_TWO);

  EXPECT_EQ(firstScore(), A_THIRD_AND_THE_REST);
  EXPECT_EQ(secondScore(), TWO_THIRDS);
}

/**
 * Given two connected players
 * When a boss worth 3 000 points is destroyed by the first, three pilots
 * having dealt it one damage each, the third not connected
 * Then the first and the second have 1 000 points each
 */
TEST_F(ScoreKeeperFixture, TheShareOfAPilotWhoLeftIsLost) {
  killBoss(FIRST_NUMBER, BOSS_WORTH, ONE_EACH_OF_THREE);

  EXPECT_EQ(firstScore(), A_THIRD_OF_THE_BOSS);
  EXPECT_EQ(secondScore(), A_THIRD_OF_THE_BOSS);
}

/**
 * Given a keeper that was destroyed
 * When a kill is published
 * Then nothing reads it
 */
TEST(ScoreKeeper, StopsListeningOnceDestroyed) {
  engine::EventBus events;
  PlayerRoster roster;
  ConnectedPlayer* const player = roster.add(
      playerEndpoint(0), std::string{PLAYER_NICKNAME}, SESSION_TOKEN);
  {
    const ScoreKeeper keeper{events, roster};
  }

  events.publish(game::EnemyKilled{.playerNumber = network::FIRST_PLAYER_NUMBER,
                                   .points = DRIFTER_WORTH});
  events.dispatch();

  EXPECT_EQ(player->score, 0U);
}

}  // namespace rtype::server::testing
