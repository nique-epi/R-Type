#include <gtest/gtest.h>
#include <optional>
#include <span>
#include "CinemaView.hpp"
#include "IntroFreeze.hpp"
#include "IntroTestConstants.hpp"
#include "IntroTimeline.hpp"
#include "IntroTimelineConstants.hpp"

using rtype::client::CinemaView;
using rtype::client::cinemaViewAt;
using rtype::client::closeupStartOf;
using rtype::client::debrisDriftAt;
using rtype::client::INTRO_CHARGE;
using rtype::client::INTRO_FIRST_KILL;
using rtype::client::INTRO_FREEZES;
using rtype::client::INTRO_SCENE_BOOST;
using rtype::client::INTRO_SCENE_SLAM;
using rtype::client::INTRO_SCENE_TITLE;
using rtype::client::INTRO_SLAM;
using rtype::client::INTRO_SLAM_TIME;
using rtype::client::INTRO_SWARM_CUT;
using rtype::client::INTRO_TITLE;
using rtype::client::IntroFreeze;
using rtype::client::sceneTimeAt;
using rtype::client::skipTargetAt;
using rtype::client::skyFactorAt;
using rtype::client::testing::A_MOMENT;
using rtype::client::testing::BEFORE_THE_FIRST_BEAT;
using rtype::client::testing::CLOSEUP_SCALE;
using rtype::client::testing::FIRST_FREEZE;
using rtype::client::testing::FLOAT_ROUNDING;
using rtype::client::testing::JUST_BEFORE_THE_SLAM;
using rtype::client::testing::MUSIC_AT_A_PRESS;
using rtype::client::testing::NEXT_BEAT_AFTER_THE_PRESS;
using rtype::client::testing::OWN_SPEED;
using rtype::client::testing::SCREEN_WIDTH;
using rtype::client::testing::SECOND_CLOSEUP_TOP_LEFT;
using rtype::client::testing::UNTIL_THE_FIRST_BEAT;
using rtype::client::testing::WARP;

/**
 * Given the freezes of the intro
 * When the music is in the first freeze
 * Then the scene stays where the freeze started
 */
TEST(IntroTimeline, TheSceneStopsDuringAFreeze) {
  EXPECT_FLOAT_EQ(sceneTimeAt(INTRO_FIRST_KILL + A_MOMENT, INTRO_FREEZES),
                  INTRO_FIRST_KILL);
}

/**
 * Given the freezes of the intro
 * When the music is past the first freeze
 * Then the scene is behind it by the 50 ms of the freeze
 */
TEST(IntroTimeline, TheSceneIsBehindTheMusicOnceAFreezeIsOver) {
  const float real = INTRO_FIRST_KILL + FIRST_FREEZE + A_MOMENT;

  EXPECT_FLOAT_EQ(sceneTimeAt(real, INTRO_FREEZES), real - FIRST_FREEZE);
}

/**
 * Given no freeze, as with REDUCE FLASHES
 * When the music is at the title
 * Then the scene is at the title too
 */
TEST(IntroTimeline, WithoutFreezesTheSceneIsTheMusic) {
  EXPECT_FLOAT_EQ(sceneTimeAt(INTRO_TITLE, std::span<const IntroFreeze>()),
                  INTRO_TITLE);
}

/**
 * Given the scene just after the boost reached its top
 * When the speed of the stars is read
 * Then they warp twelve times their speed
 */
TEST(IntroTimeline, TheStarsWarpAtTheBoost) {
  EXPECT_FLOAT_EQ(skyFactorAt(INTRO_SCENE_SLAM), WARP);
}

/**
 * Given the scene at the moment the title opens
 * When the speed of the stars is read
 * Then they are back to their own speed, the speed the title keeps
 */
TEST(IntroTimeline, TheStarsAreBackToTheirSpeedWhenTheTitleOpens) {
  EXPECT_FLOAT_EQ(skyFactorAt(INTRO_SCENE_TITLE), OWN_SPEED);
}

/**
 * Given the close-up of the second pilot
 * When its shot is read
 * Then it is zoomed three times on (240, 155)
 */
TEST(IntroTimeline, ACloseUpZoomsThreeTimesOnItsPilot) {
  EXPECT_EQ(
      cinemaViewAt(closeupStartOf(1) + A_MOMENT),
      (CinemaView{.zoom = CLOSEUP_SCALE, .topLeft = SECOND_CLOSEUP_TOP_LEFT}));
}

/**
 * Given the cut to the swarm
 * When its shot is read, then the shot of the charge
 * Then both show the whole world: no zoom between the swarm and the charge
 */
TEST(IntroTimeline, TheSwarmAndTheChargeShareTheWideShot) {
  EXPECT_EQ(cinemaViewAt(INTRO_SWARM_CUT), CinemaView{});
  EXPECT_EQ(cinemaViewAt(INTRO_CHARGE), CinemaView{});
}

/**
 * Given the scene at the boost
 * When the drift of the debris is read
 * Then they have not moved yet
 */
TEST(IntroTimeline, TheDebrisWaitForTheBoost) {
  EXPECT_FLOAT_EQ(debrisDriftAt(INTRO_SCENE_BOOST), 0.0F);
}

/**
 * Given the scene when the logo lands
 * When the drift of the debris is read
 * Then they drifted further than the width of the screen: they are gone
 */
TEST(IntroTimeline, TheDebrisAreGoneWhenTheLogoLands) {
  EXPECT_GT(debrisDriftAt(INTRO_SCENE_SLAM + INTRO_SLAM_TIME), SCREEN_WIDTH);
}

/**
 * Given no music
 * When the intro is skipped
 * Then it jumps to the slam of the logo
 */
TEST(IntroTimeline, WithoutMusicASkipJumpsToTheSlam) {
  EXPECT_FLOAT_EQ(skipTargetAt(A_MOMENT, std::nullopt), INTRO_SLAM);
}

/**
 * Given music 1.23 s in, between two beats
 * When the intro is skipped
 * Then it jumps as far before the slam as the music is from its next beat,
 * so the logo slams on that beat
 */
TEST(IntroTimeline, ASkipLandsTheSlamOnTheNextBeatOfTheMusic) {
  EXPECT_NEAR(skipTargetAt(MUSIC_AT_A_PRESS, MUSIC_AT_A_PRESS),
              INTRO_SLAM - (NEXT_BEAT_AFTER_THE_PRESS - MUSIC_AT_A_PRESS),
              FLOAT_ROUNDING);
}

/**
 * Given music at the start of its file, on its first beat
 * When the intro is skipped
 * Then it jumps to the slam at once
 */
TEST(IntroTimeline, ASkipOnABeatJumpsToTheSlam) {
  EXPECT_FLOAT_EQ(skipTargetAt(0.0F, 0.0F), INTRO_SLAM);
}

/**
 * Given an intro just before the slam, and music whose next beat would
 * put the jump earlier
 * When the intro is skipped
 * Then it stays where it is: a skip never moves the intro back
 */
TEST(IntroTimeline, ASkipNeverMovesTheIntroBack) {
  EXPECT_FLOAT_EQ(skipTargetAt(JUST_BEFORE_THE_SLAM, MUSIC_AT_A_PRESS),
                  JUST_BEFORE_THE_SLAM);
}

/**
 * Given music 50 ms before its first beat
 * When the intro is skipped
 * Then the logo waits those 50 ms, so it slams on the first beat
 */
TEST(IntroTimeline, ASkipBeforeTheFirstBeatWaitsForIt) {
  EXPECT_NEAR(skipTargetAt(0.0F, BEFORE_THE_FIRST_BEAT),
              INTRO_SLAM - UNTIL_THE_FIRST_BEAT, FLOAT_ROUNDING);
}
