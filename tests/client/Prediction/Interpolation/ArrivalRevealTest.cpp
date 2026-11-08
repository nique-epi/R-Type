#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "ArrivalRevealFixture.hpp"
#include "ArrivalRevealTestConstants.hpp"
#include "FixedEntityTypeSourceConstants.hpp"
#include "RecordingSoundPlayer.hpp"
#include "SoundConstants.hpp"

using rtype::client::ENEMY_SHOT_SOUND_ID;
using rtype::client::PLAYER_SHOT_SOUND_ID;
using rtype::client::testing::ArrivalRevealFixture;
using rtype::client::testing::ENEMY_MISSILE_TYPE_NUMBER;
using rtype::client::testing::FRAMES_SHOWN;
using rtype::client::testing::PLAYER_MISSILE_TYPE_NUMBER;
using ::testing::ElementsAre;
using ::testing::IsEmpty;

/**
 * Given a player missile announced and listed by the states of ticks 30 and
 * 33, and a clock showing tick 27
 * When one frame is shown, at tick 28
 * Then the missile is not drawn and its shot does not play yet
 */
TEST_F(ArrivalRevealFixture, AMissileWaitsForTheTickShownToReachItsState) {
  fireTheMissileOf(PLAYER_MISSILE_TYPE_NUMBER);

  showOneMoreFrame();

  EXPECT_FALSE(missileIsDrawn());
  EXPECT_THAT(sounds().played(), IsEmpty());
}

/**
 * Given a player missile announced and listed by the states of ticks 30 and
 * 33
 * When frames are shown from tick 28 to tick 36
 * Then the missile is drawn and its shot plays, once
 */
TEST_F(ArrivalRevealFixture, AMissileAppearsWithItsShotOnce) {
  fireTheMissileOf(PLAYER_MISSILE_TYPE_NUMBER);

  for (int frame = 0; frame < FRAMES_SHOWN; ++frame) {
    showOneMoreFrame();
  }

  EXPECT_TRUE(missileIsDrawn());
  EXPECT_THAT(sounds().played(), ElementsAre(PLAYER_SHOT_SOUND_ID));
}

/**
 * Given a missile of an enemy announced and listed by the states of ticks 30
 * and 33
 * When frames are shown from tick 28 to tick 36
 * Then the shot of an enemy plays, once
 */
TEST_F(ArrivalRevealFixture, AMissileOfAnEnemyAppearsWithTheShotOfAnEnemy) {
  fireTheMissileOf(ENEMY_MISSILE_TYPE_NUMBER);

  for (int frame = 0; frame < FRAMES_SHOWN; ++frame) {
    showOneMoreFrame();
  }

  EXPECT_THAT(sounds().played(), ElementsAre(ENEMY_SHOT_SOUND_ID));
}

/**
 * Given a missile announced and listed by the states of ticks 30 and 33, 16
 * units further each tick
 * When frames are shown from tick 28 to tick 36
 * Then every frame that draws it after the first draws it further on: it
 * never stands still where it left the muzzle
 */
TEST_F(ArrivalRevealFixture, ADrawnMissileMovesOnEveryFrame) {
  fireTheMissileOf(PLAYER_MISSILE_TYPE_NUMBER);

  std::vector<float> drawnAt;
  for (int frame = 0; frame < FRAMES_SHOWN; ++frame) {
    showOneMoreFrame();
    if (missileIsDrawn()) {
      drawnAt.push_back(missilePosition().x);
    }
  }

  ASSERT_GE(drawnAt.size(), 2U);
  for (std::size_t frame = 1; frame < drawnAt.size(); ++frame) {
    EXPECT_GT(drawnAt[frame], drawnAt[frame - 1]) << "frame " << frame;
  }
}
