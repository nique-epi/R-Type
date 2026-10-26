#include <gtest/gtest.h>
#include <algorithm>
#include <span>
#include <vector>
#include "IntroCastBuilder.hpp"
#include "IntroCue.hpp"
#include "IntroCues.hpp"
#include "IntroCuesFixtures.hpp"
#include "IntroFreeze.hpp"
#include "IntroTestConstants.hpp"
#include "IntroTimelineConstants.hpp"
#include "SoundConstants.hpp"

using rtype::client::buildIntroCast;
using rtype::client::BYDO_GROWL_SOUND_ID;
using rtype::client::ENEMY_SHOT_SOUND_ID;
using rtype::client::FLYBY_SOUND_ID;
using rtype::client::INTRO_FREEZES;
using rtype::client::IntroCue;
using rtype::client::introCues;
using rtype::client::IntroFreeze;
using rtype::client::LOGO_IMPACT_SOUND_ID;
using rtype::client::PLAYER_SHOT_SOUND_ID;
using rtype::client::SMALL_EXPLOSION_SOUND_ID;
using rtype::client::testing::ENEMY_SHOTS;
using rtype::client::testing::FLYBYS;
using rtype::client::testing::INTRO_TEST_SEED;
using rtype::client::testing::LOGO_LANDS;
using rtype::client::testing::PLAYER_SHOTS;
using rtype::client::testing::SMALL_EXPLOSIONS;

/**
 * Given the cues of an intro
 * When the first one is read
 * Then it is the growl of the Bydo in the dark, at once
 */
TEST(IntroCues, TheIntroOpensOnTheGrowlOfTheBydo) {
  const std::vector<IntroCue> cues =
      introCues(buildIntroCast(INTRO_TEST_SEED), INTRO_FREEZES);

  ASSERT_FALSE(cues.empty());
  EXPECT_FLOAT_EQ(cues.front().at, 0.0F);
  EXPECT_EQ(cues.front().sound, BYDO_GROWL_SOUND_ID);
}

/**
 * Given the cues of an intro
 * When they are read
 * Then they come in the order of the scene
 */
TEST(IntroCues, TheCuesComeInOrder) {
  const std::vector<IntroCue> cues =
      introCues(buildIntroCast(INTRO_TEST_SEED), INTRO_FREEZES);

  EXPECT_TRUE(std::ranges::is_sorted(cues, {}, &IntroCue::at));
}

/**
 * Given the cues of an intro
 * When the flybys are counted
 * Then each of the four close-ups has one
 */
TEST(IntroCues, EachCloseUpHasItsFlyby) {
  const std::vector<IntroCue> cues =
      introCues(buildIntroCast(INTRO_TEST_SEED), INTRO_FREEZES);

  EXPECT_EQ(cuesPlaying(cues, FLYBY_SOUND_ID), FLYBYS);
}

/**
 * Given the cues of an intro
 * When the shots of players are counted
 * Then there are the shot at the prey and the 16 of the cascade
 */
TEST(IntroCues, EveryMissileOfAPlayerFiresWithASound) {
  const std::vector<IntroCue> cues =
      introCues(buildIntroCast(INTRO_TEST_SEED), INTRO_FREEZES);

  EXPECT_EQ(cuesPlaying(cues, PLAYER_SHOT_SOUND_ID), PLAYER_SHOTS);
}

/**
 * Given the cues of an intro
 * When the shots of enemies are counted
 * Then there are the shot of the Bydo in the dark and the two of the helix
 */
TEST(IntroCues, EveryMissileOfAnEnemyFiresWithASound) {
  const std::vector<IntroCue> cues =
      introCues(buildIntroCast(INTRO_TEST_SEED), INTRO_FREEZES);

  EXPECT_EQ(cuesPlaying(cues, ENEMY_SHOT_SOUND_ID), ENEMY_SHOTS);
}

/**
 * Given the cues of an intro
 * When the small explosions are counted
 * Then the prey and the 16 Bydos of the helix explode with a sound
 */
TEST(IntroCues, EveryBydoShotDownExplodesWithASound) {
  const std::vector<IntroCue> cues =
      introCues(buildIntroCast(INTRO_TEST_SEED), INTRO_FREEZES);

  EXPECT_EQ(cuesPlaying(cues, SMALL_EXPLOSION_SOUND_ID), SMALL_EXPLOSIONS);
}

/**
 * Given the cues of an intro without freezes, as with REDUCE FLASHES
 * When the impact of the logo is found
 * Then it plays when the logo lands, 0.3 s after the ninth beat
 */
TEST(IntroCues, TheLogoLandsWithItsImpact) {
  const std::vector<IntroCue> cues = introCues(buildIntroCast(INTRO_TEST_SEED),
                                               std::span<const IntroFreeze>());

  const auto impact = std::ranges::find(
      cues, std::string_view(LOGO_IMPACT_SOUND_ID), &IntroCue::sound);
  ASSERT_NE(impact, cues.end());
  EXPECT_FLOAT_EQ(impact->at, LOGO_LANDS);
}
