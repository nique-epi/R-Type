#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "RouletteScreenFixtures.hpp"
#include "RouletteScreenTestConstants.hpp"
#include "SoundConstants.hpp"
#include "WheelSoundsFixture.hpp"
#include "WheelSoundsTestConstants.hpp"

using rtype::client::ROULETTE_DROP_SOUND_ID;
using rtype::client::ROULETTE_SPIN_SOUND_ID;
using rtype::client::testing::A_ROUND;
using rtype::client::testing::bettingRoom;
using rtype::client::testing::ONE_SECOND_BEFORE_THE_RESULT;
using rtype::client::testing::spinningRoom;
using rtype::client::testing::TWO_SECONDS_BEFORE_THE_RESULT;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given a table whose bets are open
 * When the bets close and the wheel starts to spin
 * Then the ball is thrown, and nothing else plays
 */
TEST_F(WheelSoundsFixture, TheBallIsThrownAsTheWheelSpins) {
  see(bettingRoom(A_ROUND));

  see(spinningRoom());

  EXPECT_THAT(played(), ElementsAre(ROULETTE_SPIN_SOUND_ID));
}

/**
 * Given a wheel that spins, its ball still on its track
 * When the ball drops in its pocket
 * Then it bounces there
 */
TEST_F(WheelSoundsFixture, TheBallBouncesAsItDrops) {
  see(spinningWith(TWO_SECONDS_BEFORE_THE_RESULT));

  see(spinningWith(ONE_SECOND_BEFORE_THE_RESULT));

  EXPECT_THAT(played(), ElementsAre(ROULETTE_DROP_SOUND_ID));
}

/**
 * Given a wheel that spins, its ball dropped in its pocket
 * When the next frame shows it there still
 * Then nothing plays again
 */
TEST_F(WheelSoundsFixture, TheBallBouncesOnce) {
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
TEST_F(WheelSoundsFixture, ATableOpenedDuringTheSpinThrowsNothing) {
  see(spinningRoom());

  EXPECT_THAT(played(), IsEmpty());
}

/**
 * Given a player who asked for less motion, whose wheel stays still
 * When the bets close, then the result comes
 * Then nothing plays
 */
TEST_F(WheelSoundsFixture, LessMotionPlaysNothing) {
  seeWithLessMotion(bettingRoom(A_ROUND));
  seeWithLessMotion(spinningWith(TWO_SECONDS_BEFORE_THE_RESULT));

  seeWithLessMotion(spinningWith(ONE_SECOND_BEFORE_THE_RESULT));

  EXPECT_THAT(played(), IsEmpty());
}
