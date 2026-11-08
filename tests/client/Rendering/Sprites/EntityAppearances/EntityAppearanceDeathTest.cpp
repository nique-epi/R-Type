#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include <string_view>
#include "EngineException.hpp"
#include "EntityAppearance.hpp"
#include "EntityAppearanceDeathTestConstants.hpp"
#include "EntityAppearances.hpp"
#include "EntityAppearancesFixtures.hpp"
#include "EntityAppearancesTestConstants.hpp"
#include "EntityTypesTestConstants.hpp"
#include "SheetPixels.hpp"
#include "SpriteFrame.hpp"

using rtype::client::EntityAppearance;
using rtype::client::EntityAppearances;
using rtype::engine::UnknownDataFileKeyException;
using rtype::game::SpriteFrame;
using rtype::game::testing::SHIPPED_SHEET_NAMES;
using ::testing::ElementsAreArray;
using ::testing::SizeIs;

/**
 * Given a sheet whose death starts at (0, 26), three frames of 48 by 48
 * pixels 2 pixels apart
 * When its appearance is read
 * Then its death frames are at left 0, 50 and 100 of that row
 */
TEST(EntityAppearanceDeath, ReadsTheDeathFramesAlongTheirRow) {
  const EntityAppearances appearances =
      appearancesReading(SHEET_WITH_A_DEATH_TEXT);
  const EntityAppearance* const read = appearances.named(READ_TYPE_NAME);
  ASSERT_NE(read, nullptr);

  EXPECT_THAT(read->deathFrames, ElementsAreArray(DEATH_FRAMES));
}

/**
 * Given a sheet whose death row starts at top 26, frames 48 pixels high and
 * rows 2 pixels apart
 * When its appearance is read
 * Then its death without flash is the row right under it, at top 76
 */
TEST(EntityAppearanceDeath, ReadsTheDeathWithoutFlashInTheRowUnderIt) {
  const EntityAppearances appearances =
      appearancesReading(SHEET_WITH_A_DEATH_TEXT);
  const EntityAppearance* const read = appearances.named(READ_TYPE_NAME);
  ASSERT_NE(read, nullptr);

  EXPECT_THAT(read->deathFramesWithoutFlash,
              ElementsAreArray(DEATH_FRAMES_WITHOUT_FLASH));
}

/**
 * Given a sheet whose death frames show 70 ms each
 * When its appearance is read
 * Then each death frame lasts 70 ms
 */
TEST(EntityAppearanceDeath, ReadsTheDeathFrameTime) {
  const EntityAppearances appearances =
      appearancesReading(SHEET_WITH_A_DEATH_TEXT);
  const EntityAppearance* const read = appearances.named(READ_TYPE_NAME);
  ASSERT_NE(read, nullptr);

  EXPECT_EQ(read->deathFrameDuration, DEATH_FRAME_TIME);
}

/**
 * Given a sheet with no death
 * When its appearance is read
 * Then it has no death frames, so its enemy keeps the Bydo explosion
 */
TEST(EntityAppearanceDeath, LeavesNoDeathFramesWithoutItsTable) {
  const EntityAppearances appearances = appearancesReading(READ_SHEET_TEXT);
  const EntityAppearance* const read = appearances.named(READ_TYPE_NAME);
  ASSERT_NE(read, nullptr);

  EXPECT_TRUE(read->deathFrames.empty());
  EXPECT_TRUE(read->deathFramesWithoutFlash.empty());
}

/**
 * Given a sheet whose death holds a key no idle animation has
 * When its appearance is read
 * Then reading stops on that key
 */
TEST(EntityAppearanceDeath, RefusesAnUnknownKeyOfTheDeath) {
  EXPECT_THROW(
      static_cast<void>(appearancesReading(DEATH_WITH_AN_UNKNOWN_KEY_TEXT)),
      UnknownDataFileKeyException);
}

/**
 * Given the sheets shipped in assets/
 * When each frame of each death, and of its version without flash, is laid on
 * the sprite sheet it names
 * Then it lies inside that sheet and shows at least one visible pixel, and
 * the pixels just around it are all transparent, as an idle frame
 */
TEST(EntityAppearanceDeath, EveryShippedDeathFrameShowsOneWholeSprite) {
  const EntityAppearances appearances = shippedAppearances();
  for (const std::string_view name : SHIPPED_SHEET_NAMES) {
    const EntityAppearance* const shipped = appearances.named(name);
    ASSERT_NE(shipped, nullptr);
    const sf::Image sheet = shippedSheet(shipped->sheet);
    for (const auto& row :
         {shipped->deathFrames, shipped->deathFramesWithoutFlash}) {
      for (const SpriteFrame& frame : row) {
        SCOPED_TRACE(name);
        SCOPED_TRACE(frame.top);
        SCOPED_TRACE(frame.left);

        ASSERT_TRUE(liesInside(frame, sheet.getSize()));
        EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
        EXPECT_FALSE(hasAVisiblePixelAround(sheet, frame));
      }
    }
  }
}

/**
 * Given the sheets shipped in assets/
 * When the Voltjelly, the Reaper, the Drillback and the Bydomancer are read
 * Then each draws a death of six frames, as the Bydo explosion
 */
TEST(EntityAppearanceDeath, TheFourNewEnemiesDrawTheirOwnDeath) {
  const EntityAppearances appearances = shippedAppearances();
  for (const std::string_view name : ENEMIES_WITH_A_DEATH) {
    SCOPED_TRACE(name);
    const EntityAppearance* const shipped = appearances.named(name);
    ASSERT_NE(shipped, nullptr);

    EXPECT_THAT(shipped->deathFrames, SizeIs(SHIPPED_DEATH_FRAME_COUNT));
    EXPECT_THAT(shipped->deathFramesWithoutFlash,
                SizeIs(SHIPPED_DEATH_FRAME_COUNT));
  }
}

/**
 * Given the sheets shipped in assets/
 * When the Bydo is read
 * Then it draws no death, so it keeps the Bydo explosion of the effect sheet
 */
TEST(EntityAppearanceDeath, TheBydoKeepsTheBydoExplosion) {
  const EntityAppearances appearances = shippedAppearances();
  const EntityAppearance* const bydo = appearances.named(BYDO_SHEET_NAME);
  ASSERT_NE(bydo, nullptr);

  EXPECT_TRUE(bydo->deathFrames.empty());
}
