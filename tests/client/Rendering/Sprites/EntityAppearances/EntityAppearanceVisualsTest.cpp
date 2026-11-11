#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "EngineException.hpp"
#include "EntityAppearance.hpp"
#include "EntityAppearanceVisualsTestConstants.hpp"
#include "EntityAppearances.hpp"
#include "EntityAppearancesFixtures.hpp"
#include "EntityAppearancesTestConstants.hpp"
#include "NamedEffect.hpp"

using rtype::client::EntityAppearance;
using rtype::client::EntityAppearances;
using rtype::client::NamedEffect;
using rtype::engine::UnknownDataFileKeyException;
using ::testing::ElementsAreArray;
using ::testing::SizeIs;

/**
 * Given a sheet whose effect `spark` is two frames from (0, 30), 4 pixels
 * apart, each 60 ms, 8 pixels above the body
 * When its appearance is read
 * Then the effect has those frames, the row under them without flash, that
 * time and that anchor
 */
TEST(EntityAppearanceVisuals, ReadsANamedEffect) {
  const EntityAppearances appearances =
      appearancesReading(SHEET_WITH_VISUALS_TEXT);
  const EntityAppearance* const read = appearances.named(READ_TYPE_NAME);
  ASSERT_NE(read, nullptr);
  ASSERT_THAT(read->effects, SizeIs(1));

  const NamedEffect& spark = read->effects.front();
  EXPECT_EQ(spark.name, SPARK_EFFECT);
  EXPECT_THAT(spark.frames, ElementsAreArray(SPARK_FRAMES));
  EXPECT_THAT(spark.framesWithoutFlash,
              ElementsAreArray(SPARK_FRAMES_WITHOUT_FLASH));
  EXPECT_EQ(spark.frameDuration, SPARK_FRAME_TIME);
  EXPECT_EQ(spark.anchor.y, SPARK_ANCHOR_Y);
}

/**
 * Given a sheet whose looks are given `cracked` first, then `angry`
 * When its appearance is read
 * Then the looks are in the order of their names, as the server numbers
 * them, each with its frames and time
 */
TEST(EntityAppearanceVisuals, ReadsTheLooksInTheOrderOfTheirNames) {
  const EntityAppearances appearances =
      appearancesReading(SHEET_WITH_VISUALS_TEXT);
  const EntityAppearance* const read = appearances.named(READ_TYPE_NAME);
  ASSERT_NE(read, nullptr);
  ASSERT_THAT(read->looks, SizeIs(TWO_LOOKS));

  EXPECT_THAT(read->looks[0].frames, ElementsAreArray(ANGRY_FRAMES));
  EXPECT_THAT(read->looks[1].frames, ElementsAreArray(CRACKED_FRAMES));
  EXPECT_EQ(read->looks[1].frameDuration, CRACKED_FRAME_TIME);
}

/**
 * Given a sheet naming no effect and no look
 * When its appearance is read
 * Then it has neither
 */
TEST(EntityAppearanceVisuals, LeavesNoVisualsWithoutTheirTables) {
  const EntityAppearances appearances = appearancesReading(READ_SHEET_TEXT);
  const EntityAppearance* const read = appearances.named(READ_TYPE_NAME);
  ASSERT_NE(read, nullptr);

  EXPECT_TRUE(read->effects.empty());
  EXPECT_TRUE(read->looks.empty());
}

/**
 * Given a sheet whose effect holds a key an effect does not have
 * When its appearance is read
 * Then reading stops on that key
 */
TEST(EntityAppearanceVisuals, RefusesAnUnknownKeyOfAnEffect) {
  EXPECT_THROW(
      static_cast<void>(appearancesReading(EFFECT_WITH_AN_UNKNOWN_KEY_TEXT)),
      UnknownDataFileKeyException);
}
