#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "EngineException.hpp"
#include "EntityAppearance.hpp"
#include "EntityAppearanceEnragedTestConstants.hpp"
#include "EntityAppearances.hpp"
#include "EntityAppearancesFixtures.hpp"
#include "EntityAppearancesTestConstants.hpp"

using rtype::client::EntityAppearance;
using rtype::client::EntityAppearances;
using rtype::engine::UnknownDataFileKeyException;
using ::testing::ElementsAreArray;

/**
 * Given a sheet whose enraged idle starts at (0, 130), two frames of 128 by
 * 128 pixels 2 pixels apart
 * When its appearance is read
 * Then its enraged idle frames are at left 0 and 130 of that row
 */
TEST(EntityAppearanceEnraged, ReadsTheEnragedIdleFramesAlongTheirRow) {
  const EntityAppearances appearances =
      appearancesReading(SHEET_WITH_AN_ENRAGED_IDLE_TEXT);
  const EntityAppearance* const read = appearances.named(READ_TYPE_NAME);
  ASSERT_NE(read, nullptr);

  EXPECT_THAT(read->enragedIdleFrames, ElementsAreArray(ENRAGED_IDLE_FRAMES));
}

/**
 * Given a sheet whose enraged idle frames show 120 ms each
 * When its appearance is read
 * Then each enraged idle frame lasts 120 ms
 */
TEST(EntityAppearanceEnraged, ReadsTheEnragedIdleFrameTime) {
  const EntityAppearances appearances =
      appearancesReading(SHEET_WITH_AN_ENRAGED_IDLE_TEXT);
  const EntityAppearance* const read = appearances.named(READ_TYPE_NAME);
  ASSERT_NE(read, nullptr);

  EXPECT_EQ(read->enragedIdleFrameDuration, ENRAGED_IDLE_FRAME_TIME);
}

/**
 * Given a sheet with no enraged idle
 * When its appearance is read
 * Then it has no enraged idle frames
 */
TEST(EntityAppearanceEnraged, LeavesNoEnragedFramesWithoutItsTable) {
  const EntityAppearances appearances = appearancesReading(READ_SHEET_TEXT);
  const EntityAppearance* const read = appearances.named(READ_TYPE_NAME);
  ASSERT_NE(read, nullptr);

  EXPECT_TRUE(read->enragedIdleFrames.empty());
}

/**
 * Given a sheet whose enraged idle holds a key no idle animation has
 * When its appearance is read
 * Then reading stops on that key
 */
TEST(EntityAppearanceEnraged, RefusesAnUnknownKeyOfTheEnragedIdle) {
  EXPECT_THROW(static_cast<void>(
                   appearancesReading(ENRAGED_IDLE_WITH_AN_UNKNOWN_KEY_TEXT)),
               UnknownDataFileKeyException);
}
