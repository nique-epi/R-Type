#include <gtest/gtest.h>
#include <optional>
#include "FlashPreference.hpp"
#include "NamedEffectScene.hpp"
#include "NamedEffectTestConstants.hpp"
#include "Playback.hpp"

using rtype::client::FlashPreference;
using rtype::client::Playback;
using rtype::client::testing::EFFECT_BODY_ID;
using rtype::client::testing::NamedEffectScene;
using rtype::client::testing::SPARK;
using rtype::client::testing::SPARK_ANCHOR;
using rtype::client::testing::SPARK_FIRST_FRAME;
using rtype::client::testing::SPARK_FIRST_FRAME_WITHOUT_FLASH;
using rtype::client::testing::UNKNOWN_BODY_ID;
using rtype::client::testing::UNKNOWN_EFFECT;

/**
 * Given a body whose sheet names the effect spark
 * When the server names spark on that body
 * Then one effect of the sheet of the body shows the first frame of spark on
 * the body, played once, hanging by the anchor of the effect
 */
TEST(EffectPlayer, PlaysTheEffectTheServerNamesOnTheBody) {
  NamedEffectScene scene;

  scene.play(EFFECT_BODY_ID, SPARK);

  EXPECT_EQ(scene.effectFrame(), std::optional{SPARK_FIRST_FRAME});
  EXPECT_EQ(scene.effectAnimation().animation.playback, Playback::Once);
  EXPECT_EQ(scene.effectAnimation().animation.anchor, SPARK_ANCHOR);
}

/**
 * Given a player who reduces the flashes
 * When the server names spark on the body
 * Then the effect shows its frame without flash, in the row under it
 */
TEST(EffectPlayer, PlaysTheVersionWithoutFlashWhenFlashesAreReduced) {
  NamedEffectScene scene;
  scene.preferFlashes(FlashPreference::Reduced);

  scene.play(EFFECT_BODY_ID, SPARK);

  EXPECT_EQ(scene.effectFrame(),
            std::optional{SPARK_FIRST_FRAME_WITHOUT_FLASH});
}

/**
 * Given a body whose sheet names spark only
 * When the server names another effect on it
 * Then nothing plays
 */
TEST(EffectPlayer, PlaysNothingForAnEffectTheSheetDoesNotName) {
  NamedEffectScene scene;

  scene.play(EFFECT_BODY_ID, UNKNOWN_EFFECT);

  EXPECT_EQ(scene.effectCount(), 0U);
}

/**
 * Given a client that has one body
 * When the server names spark on a body the client does not have
 * Then nothing plays
 */
TEST(EffectPlayer, PlaysNothingOnABodyTheClientDoesNotHave) {
  NamedEffectScene scene;

  scene.play(UNKNOWN_BODY_ID, SPARK);

  EXPECT_EQ(scene.effectCount(), 0U);
}
