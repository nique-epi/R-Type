#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "RouletteSceneTestConstants.hpp"
#include "RouletteScreenFixtures.hpp"
#include "RouletteScreenTestConstants.hpp"
#include "RoundSoundsFixture.hpp"
#include "RoundSoundsTestConstants.hpp"
#include "SoundConstants.hpp"

using rtype::client::ROULETTE_DROP_SOUND_ID;
using rtype::client::ROULETTE_JACKPOT_SOUND_ID;
using rtype::client::ROULETTE_SPIN_SOUND_ID;
using rtype::client::testing::A_ROUND;
using rtype::client::testing::bettingRoom;
using rtype::client::testing::ONE_SECOND_BEFORE_THE_RESULT;
using rtype::client::testing::PAYOUT_AT_THE_THRESHOLD;
using rtype::client::testing::PAYOUT_SHORT_OF_THE_THRESHOLD;
using rtype::client::testing::resultRoom;
using rtype::client::testing::spinningRoom;
using rtype::client::testing::TWO_SECONDS_BEFORE_THE_RESULT;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given a table whose bets are open
 * When the bets close and the wheel starts to spin
 * Then the ball is thrown, and nothing else plays
 */
TEST_F(RoundSoundsFixture, TheBallIsThrownAsTheWheelSpins) {
  see(bettingRoom(A_ROUND));

  see(spinningRoom());

  EXPECT_THAT(played(), ElementsAre(ROULETTE_SPIN_SOUND_ID));
}

/**
 * Given a wheel that spins, its ball still on its track
 * When the ball drops in its pocket
 * Then it bounces there
 */
TEST_F(RoundSoundsFixture, TheBallBouncesAsItDrops) {
  see(spinningWith(TWO_SECONDS_BEFORE_THE_RESULT));

  see(spinningWith(ONE_SECOND_BEFORE_THE_RESULT));

  EXPECT_THAT(played(), ElementsAre(ROULETTE_DROP_SOUND_ID));
}

/**
 * Given a wheel that spins, its ball dropped in its pocket
 * When the next frame shows it there still
 * Then nothing plays again
 */
TEST_F(RoundSoundsFixture, TheBallBouncesOnce) {
  see(spinningWith(TWO_SECONDS_BEFORE_THE_RESULT));
  see(spinningWith(ONE_SECOND_BEFORE_THE_RESULT));

  see(spinningWith(ONE_SECOND_BEFORE_THE_RESULT));

  EXPECT_THAT(played(), ElementsAre(ROULETTE_DROP_SOUND_ID));
}

/**
 * Given a table just opened
 * When its first frame shows the wheel already spinning
 * Then nothing plays, since the throw was not seen
 */
TEST_F(RoundSoundsFixture, ATableOpenedDuringTheSpinThrowsNothing) {
  see(spinningRoom());

  EXPECT_THAT(played(), IsEmpty());
}

/**
 * Given a player who asked for less motion, whose wheel stays still
 * When the bets close, then the result comes
 * Then nothing plays
 */
TEST_F(RoundSoundsFixture, LessMotionPlaysNothing) {
  seeWithLessMotion(bettingRoom(A_ROUND));
  seeWithLessMotion(spinningWith(TWO_SECONDS_BEFORE_THE_RESULT));

  seeWithLessMotion(spinningWith(ONE_SECOND_BEFORE_THE_RESULT));

  EXPECT_THAT(played(), IsEmpty());
}

/**
 * Given a wheel whose ball lies in its pocket
 * When the result comes with a big win
 * Then the jackpot plays
 */
TEST_F(RoundSoundsFixture, ABigWinPlaysTheJackpot) {
  see(spinningWith(ONE_SECOND_BEFORE_THE_RESULT));

  see(resultRoom(PAYOUT_AT_THE_THRESHOLD));

  EXPECT_THAT(played(), ElementsAre(ROULETTE_JACKPOT_SOUND_ID));
}

/**
 * Given a wheel whose ball lies in its pocket
 * When the result comes with a win short of a big one
 * Then nothing plays
 */
TEST_F(RoundSoundsFixture, ASmallerWinPlaysNoJackpot) {
  see(spinningWith(ONE_SECOND_BEFORE_THE_RESULT));

  see(resultRoom(PAYOUT_SHORT_OF_THE_THRESHOLD));

  EXPECT_THAT(played(), IsEmpty());
}

/**
 * Given a player who asked for less motion, whose wheel stays still
 * When the result comes with a big win
 * Then the jackpot plays all the same
 */
TEST_F(RoundSoundsFixture, LessMotionStillPlaysTheJackpot) {
  seeWithLessMotion(spinningWith(ONE_SECOND_BEFORE_THE_RESULT));

  seeWithLessMotion(resultRoom(PAYOUT_AT_THE_THRESHOLD));

  EXPECT_THAT(played(), ElementsAre(ROULETTE_JACKPOT_SOUND_ID));
}
