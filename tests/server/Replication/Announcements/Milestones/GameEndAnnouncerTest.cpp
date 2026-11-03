#include <gtest/gtest.h>
#include <cstddef>
#include <initializer_list>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "GameEndAnnouncerFixture.hpp"
#include "GameEndAnnouncerTestConstants.hpp"
#include "GameEnded.hpp"
#include "Instances/GameInstanceTestConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "SentDatagrams.hpp"

namespace rtype::server::testing {

/**
 * Given two connected players with their scores, whose game just ended
 * When the first tick of the finished game passes
 * Then each player is sent one result listing both, in order of number, with
 * their scores
 */
TEST_F(GameEndAnnouncerFixture, SendsTheResultAtTheFirstFinishedTick) {
  join(FIRST_PLAYER).score = FIRST_PLAYER_FINAL_SCORE;
  join(SECOND_PLAYER).score = SECOND_PLAYER_FINAL_SCORE;
  endTheGame();

  runFinishedTicks(1);

  for (const std::size_t player : {FIRST_PLAYER, SECOND_PLAYER}) {
    const std::vector<network::OutgoingDatagram> received = sentTo(player);
    ASSERT_EQ(received.size(), 1U);
    EXPECT_EQ(onlyMessageIn<network::GameEnded>(received.front()),
              resultOfBothPlayers());
  }
}

/**
 * Given a player whose game ended, sent the result at the first finished tick
 * When the next tick passes
 * Then nothing more is sent: the result is not sent every tick
 */
TEST_F(GameEndAnnouncerFixture, DoesNotSendTheResultAgainAtTheNextTick) {
  join(FIRST_PLAYER);
  endTheGame();
  runFinishedTicks(1);

  runFinishedTicks(1);

  EXPECT_EQ(sentTo(FIRST_PLAYER).size(), 1U);
}

/**
 * Given a player whose game ended, sent the result at the first finished tick
 * When the 500 ms the protocol sets between two GameEnded pass
 * Then the player was sent the result once more, so a lost one is corrected
 */
TEST_F(GameEndAnnouncerFixture, SendsTheResultAgainOnceTheIntervalPassed) {
  join(FIRST_PLAYER);
  endTheGame();
  runFinishedTicks(1);

  runFinishedTicks(PROTOCOL_GAME_ENDED_TICKS);

  EXPECT_EQ(sentTo(FIRST_PLAYER).size(), 2U);
}

/**
 * Given two players whose game ended with their scores, sent the result
 * When the first player is credited points, then the interval passes
 * Then the result sent again still holds the scores the game ended with
 */
TEST_F(GameEndAnnouncerFixture, KeepsTheScoresTheGameEndedWith) {
  ConnectedPlayer& first = join(FIRST_PLAYER);
  first.score = FIRST_PLAYER_FINAL_SCORE;
  join(SECOND_PLAYER).score = SECOND_PLAYER_FINAL_SCORE;
  endTheGame();
  runFinishedTicks(1);

  first.score += POINTS_AFTER_THE_END;
  runFinishedTicks(PROTOCOL_GAME_ENDED_TICKS);

  const std::vector<network::OutgoingDatagram> received = sentTo(FIRST_PLAYER);
  ASSERT_EQ(received.size(), 2U);
  EXPECT_EQ(onlyMessageIn<network::GameEnded>(received.back()),
            resultOfBothPlayers());
}

}  // namespace rtype::server::testing
