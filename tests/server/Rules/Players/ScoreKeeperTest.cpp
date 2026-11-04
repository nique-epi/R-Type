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
