#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include "MotionPreference.hpp"
#include "RoulettePhase.hpp"
#include "TimeConstants.hpp"
#include "WheelMoment.hpp"
#include "WheelMotion.hpp"
#include "WheelMotionTestConstants.hpp"
#include "WheelPicture.hpp"

using rtype::client::MotionPreference;
using rtype::client::rotorFramesLeft;
using rtype::client::stoppingFrameOf;
using rtype::client::WheelMoment;
using rtype::client::WheelPicture;
using rtype::client::wheelPictureAt;
using rtype::client::testing::A_WHOLE_SPIN;
using rtype::client::testing::BALL_SPRITE_CENTER;
using rtype::client::testing::FIVE_FRAMES_LEFT;
using rtype::client::testing::FOUR_FRAMES_LEFT;
using rtype::client::testing::FOUR_LAST_FRAMES;
using rtype::client::testing::FRAMES_OF_A_WHOLE_SPIN;
using rtype::client::testing::JUST_PAST_FOUR_FRAMES;
using rtype::client::testing::ONE_FRAME_LEFT;
using rtype::client::testing::ONE_LAST_FRAME;
using rtype::client::testing::POCKET_RADIUS;
using rtype::client::testing::ROUNDING;
using rtype::client::testing::SEVENTEEN_NUMBER;
using rtype::client::testing::SEVENTEEN_STOPS_ON;
using rtype::client::testing::TRACK_RADIUS;
using rtype::client::testing::TWENTY_SIX_NUMBER;
using rtype::client::testing::TWENTY_SIX_STOPS_ON;
using rtype::client::testing::WHEEL_MIDDLE;
using rtype::client::testing::ZERO_NUMBER;
using rtype::client::testing::ZERO_STOPS_ON;
using rtype::engine::Duration;
using rtype::network::RoulettePhase;

/**
 * Given the zero, 17 and 26
 * When the frame that brings each to the top is read
 * Then it is 74 less two frames per pocket each lies from the zero
 */
TEST(WheelMotion, EachNumberStopsOnTheFrameThatBringsItToTheTop) {
  EXPECT_EQ(stoppingFrameOf(ZERO_NUMBER), ZERO_STOPS_ON);
  EXPECT_EQ(stoppingFrameOf(SEVENTEEN_NUMBER), SEVENTEEN_STOPS_ON);
  EXPECT_EQ(stoppingFrameOf(TWENTY_SIX_NUMBER), TWENTY_SIX_STOPS_ON);
}

/**
 * Given the time left of a spin
 * When the frames the rotor still turns are counted
 * Then none at the end, one in the last 200 ms, four in the last 800 ms, a
 * fifth begun just before
 */
TEST(WheelMotion, TheRotorSlowsDownByStepsToTheEnd) {
  EXPECT_EQ(rotorFramesLeft(Duration::zero()), 0);
  EXPECT_EQ(rotorFramesLeft(ONE_LAST_FRAME), ONE_FRAME_LEFT);
  EXPECT_EQ(rotorFramesLeft(FOUR_LAST_FRAMES), FOUR_FRAMES_LEFT);
  EXPECT_EQ(rotorFramesLeft(JUST_PAST_FOUR_FRAMES), FIVE_FRAMES_LEFT);
}

/**
 * Given a whole spin of 6 s
 * When the frames the rotor turns are counted
 * Then they are the 19 frames of the slowing steps and 75 of 50 ms
 */
TEST(WheelMotion, AWholeSpinTurnsAllItsFrames) {
  EXPECT_EQ(rotorFramesLeft(A_WHOLE_SPIN), FRAMES_OF_A_WHOLE_SPIN);
}

/**
 * Given 17 drawn
 * When the spin ends
 * Then the rotor shows the frame that brings 17 to the top, the ball in
 * its pocket there, as the result shows it
 */
TEST(WheelMotion, TheWheelStopsOnTheWinningPocket) {
  const WheelPicture end =
      wheelPictureAt(WheelMoment{.phase = RoulettePhase::Spinning,
                                 .winningNumber = SEVENTEEN_NUMBER,
                                 .timeLeft = Duration::zero()},
                     MotionPreference::Full);
  const WheelPicture result =
      wheelPictureAt(WheelMoment{.phase = RoulettePhase::Result,
                                 .winningNumber = SEVENTEEN_NUMBER},
                     MotionPreference::Full);

  EXPECT_EQ(end.rotorFrame, SEVENTEEN_STOPS_ON);
  EXPECT_EQ(end, result);
  ASSERT_TRUE(end.ball.has_value());
  EXPECT_NEAR(end.ball.value_or(sf::Vector2i()).x + BALL_SPRITE_CENTER,
              WHEEL_MIDDLE, ROUNDING);
  EXPECT_NEAR(end.ball.value_or(sf::Vector2i()).y + BALL_SPRITE_CENTER,
              WHEEL_MIDDLE - POCKET_RADIUS, ROUNDING);
}

/**
 * Given a spin that just began
 * When the wheel is read
 * Then the ball runs on the track, far from the pockets
 */
TEST(WheelMotion, TheBallRunsOnTheTrackAsTheSpinBegins) {
  const WheelPicture start =
      wheelPictureAt(WheelMoment{.phase = RoulettePhase::Spinning,
                                 .winningNumber = SEVENTEEN_NUMBER,
                                 .timeLeft = A_WHOLE_SPIN},
                     MotionPreference::Full);

  ASSERT_TRUE(start.ball.has_value());
  const sf::Vector2i ball = start.ball.value_or(sf::Vector2i());
  EXPECT_NEAR(std::hypot(ball.x + BALL_SPRITE_CENTER - WHEEL_MIDDLE,
                         ball.y + BALL_SPRITE_CENTER - WHEEL_MIDDLE),
              TRACK_RADIUS, ROUNDING);
}

/**
 * Given the bets open
 * When the wheel is read
 * Then the rotor rests and no ball is in play
 */
TEST(WheelMotion, TheWheelRestsWhileTheBetsAreOpen) {
  EXPECT_EQ(wheelPictureAt(WheelMoment{.phase = RoulettePhase::Betting},
                           MotionPreference::Full),
            WheelPicture{});
}

/**
 * Given less motion asked, and a spin half done
 * When the wheel is read
 * Then the rotor rests and no ball is in play until the result
 */
TEST(WheelMotion, LessMotionKeepsTheWheelStill) {
  EXPECT_EQ(wheelPictureAt(WheelMoment{.phase = RoulettePhase::Spinning,
                                       .winningNumber = SEVENTEEN_NUMBER,
                                       .timeLeft = FOUR_LAST_FRAMES},
                           MotionPreference::Reduced),
            WheelPicture{});
}
