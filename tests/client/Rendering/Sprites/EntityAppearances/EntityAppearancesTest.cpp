#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include <array>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ActionAnimation.hpp"
#include "BackgroundConstants.hpp"
#include "EngineException.hpp"
#include "EntityAppearance.hpp"
#include "EntityAppearances.hpp"
#include "EntityAppearancesFixtures.hpp"
#include "EntityAppearancesTestConstants.hpp"
#include "EntitySheet.hpp"
#include "EntitySpriteConstants.hpp"
#include "EntitySpriteTestConstants.hpp"
#include "EntityTypesTestConstants.hpp"
#include "SheetPixels.hpp"
#include "ShippedEntitySheets.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

using rtype::client::ActionAnimation;
using rtype::client::BACKGROUND_PIXEL_SIZE;
using rtype::client::EntityAppearance;
using rtype::client::EntityAppearances;
using rtype::client::PLAYER_SHIP_FRAME;
using rtype::engine::DataFileTextExpectedException;
using rtype::engine::DataFileValueOutOfRangeException;
using rtype::engine::Duration;
using rtype::engine::MissingDataFileKeyException;
using rtype::engine::UnknownDataFileKeyException;
using rtype::game::EntitySheet;
using rtype::game::SpriteFrame;
using rtype::game::testing::BYDOMANCER_SHEET;
using rtype::game::testing::DRIFTER_SHEET;
using rtype::game::testing::DRILLBACK_SHEET;
using rtype::game::testing::ENEMY_MISSILE_SHEET;
using rtype::game::testing::ORB_SHARD_SHEET;
using rtype::game::testing::PLAYER_MISSILE_SHEET;
using rtype::game::testing::REAPER_SHEET;
using rtype::game::testing::RUNE_ORB_SHEET;
using rtype::game::testing::SHIP_SHEET;
using rtype::game::testing::SHIPPED_SHEET_NAMES;
using rtype::game::testing::shippedEntitySheets;
using rtype::game::testing::VOLTJELLY_SHEET;
using rtype::game::testing::WEAVER_SHEET;
using ::testing::ElementsAre;
using ::testing::ElementsAreArray;

/**
 * Given a sheet whose idle animation starts at (10, 20), 8 by 6 pixels, three
 * frames 2 pixels apart
 * When its appearance is read
 * Then its idle frames are at left 10, 20 and 30 of the same row
 */
TEST(EntityAppearances, ReadsTheIdleFramesAlongTheirRow) {
  const EntityAppearances appearances = appearancesReading(READ_SHEET_TEXT);
  const EntityAppearance* const read = appearances.named(READ_TYPE_NAME);
  ASSERT_NE(read, nullptr);

  EXPECT_THAT(read->idleFrames, ElementsAreArray(READ_IDLE_FRAMES));
}

/**
 * Given a sheet whose idle frames show 120 ms each
 * When its appearance is read
 * Then each frame lasts 120 ms
 */
TEST(EntityAppearances, ReadsTheFrameTimeInMilliseconds) {
  const EntityAppearances appearances = appearancesReading(READ_SHEET_TEXT);
  const EntityAppearance* const read = appearances.named(READ_TYPE_NAME);
  ASSERT_NE(read, nullptr);

  EXPECT_EQ(read->idleFrameDuration, READ_FRAME_TIME);
}

/**
 * Given a sheet drawn from sprites/voltjelly.png
 * When its appearance is read
 * Then it is drawn from that sprite sheet
 */
TEST(EntityAppearances, ReadsTheSpriteSheetItIsDrawnFrom) {
  const EntityAppearances appearances = appearancesReading(READ_SHEET_TEXT);
  const EntityAppearance* const read = appearances.named(READ_TYPE_NAME);
  ASSERT_NE(read, nullptr);

  EXPECT_EQ(read->sheet, READ_SHEET_ID);
}

/**
 * Given a sheet whose `[gameplay]` holds a key the server alone knows
 * When the client reads its appearance
 * Then it reads it: the client never reads the keys of `[gameplay]`
 */
TEST(EntityAppearances, LeavesTheGameplayToTheServer) {
  const EntityAppearances appearances = appearancesReading(READ_SHEET_TEXT);

  EXPECT_NE(appearances.named(READ_TYPE_NAME), nullptr);
}

/**
 * Given one sheet read
 * When the appearance of another name is looked up
 * Then there is none
 */
TEST(EntityAppearances, KnowsNoAppearanceForAnotherName) {
  const EntityAppearances appearances = appearancesReading(READ_SHEET_TEXT);

  EXPECT_EQ(appearances.named(UNREAD_TYPE_NAME), nullptr);
}

/**
 * Given three sheets, two of them drawn from the same sprite sheet
 * When the sprite sheets they need are listed
 * Then each is listed once, in alphabetical order
 */
TEST(EntityAppearances, ListsEachSpriteSheetOnce) {
  const EntityAppearances appearances = threeAppearancesOnTwoSheets();

  EXPECT_THAT(appearances.sheets(), ElementsAre(std::string{OTHER_SHEET_ID},
                                                std::string{READ_SHEET_ID}));
}

/**
 * Given a sheet whose `[appearance]` has no idle table
 * When it is read
 * Then reading stops on the missing key
 */
TEST(EntityAppearances, RefusesASheetWithoutAnIdleAnimation) {
  EXPECT_THROW(static_cast<void>(appearancesReading(SHEET_WITHOUT_IDLE_TEXT)),
               MissingDataFileKeyException);
}

/**
 * Given a sheet with no `[gameplay]`
 * When the client reads it
 * Then reading stops on the missing key: the sheet would give the server
 * nothing to play
 */
TEST(EntityAppearances, RefusesASheetWithoutGameplay) {
  EXPECT_THROW(
      static_cast<void>(appearancesReading(SHEET_WITHOUT_GAMEPLAY_TEXT)),
      MissingDataFileKeyException);
}

/**
 * Given a sheet with a table at its root besides `[gameplay]` and
 * `[appearance]`
 * When it is read
 * Then reading stops on the unknown key
 */
TEST(EntityAppearances, RefusesAnUnknownTableAtTheRoot) {
  EXPECT_THROW(static_cast<void>(
                   appearancesReading(SHEET_WITH_AN_UNKNOWN_ROOT_TABLE_TEXT)),
               UnknownDataFileKeyException);
}

/**
 * Given a sheet whose `[appearance]` holds a key it does not know
 * When it is read
 * Then reading stops on that key, so a misspelt key never goes unnoticed
 */
TEST(EntityAppearances, RefusesAnUnknownKeyOfTheAppearance) {
  EXPECT_THROW(static_cast<void>(appearancesReading(
                   SHEET_WITH_AN_UNKNOWN_APPEARANCE_KEY_TEXT)),
               UnknownDataFileKeyException);
}

/**
 * Given a sheet whose idle table holds a key it does not know
 * When it is read
 * Then reading stops on that key
 */
TEST(EntityAppearances, RefusesAnUnknownKeyOfTheIdleAnimation) {
  EXPECT_THROW(static_cast<void>(
                   appearancesReading(SHEET_WITH_AN_UNKNOWN_IDLE_KEY_TEXT)),
               UnknownDataFileKeyException);
}

/**
 * Given a sheet whose idle animation holds no frame
 * When it is read
 * Then reading stops on the frame count, out of its range
 */
TEST(EntityAppearances, RefusesAnIdleAnimationOfNoFrame) {
  EXPECT_THROW(static_cast<void>(appearancesReading(SHEET_WITH_NO_FRAME_TEXT)),
               DataFileValueOutOfRangeException);
}

/**
 * Given a sheet whose idle animation holds 16 frames, the most it may
 * When it is read
 * Then it holds 16 frames
 */
TEST(EntityAppearances, ReadsTheMostFramesAnIdleAnimationMayHold) {
  const EntityAppearances appearances =
      appearancesReading(SHEET_WITH_SIXTEEN_FRAMES_TEXT);
  const EntityAppearance* const read = appearances.named(READ_TYPE_NAME);
  ASSERT_NE(read, nullptr);

  EXPECT_EQ(read->idleFrames.size(), MOST_FRAMES_A_SHEET_HOLDS);
}

/**
 * Given a sheet whose idle animation holds 17 frames, one past the most
 * When it is read
 * Then reading stops on the frame count
 */
TEST(EntityAppearances, RefusesOneFramePastTheMost) {
  EXPECT_THROW(
      static_cast<void>(appearancesReading(SHEET_WITH_SEVENTEEN_FRAMES_TEXT)),
      DataFileValueOutOfRangeException);
}

/**
 * Given a sheet whose frame time is written 120.0
 * When it is read
 * Then reading stops: a number of a sheet is an integer
 */
TEST(EntityAppearances, RefusesAFractionalFrameTime) {
  EXPECT_THROW(static_cast<void>(
                   appearancesReading(SHEET_WITH_A_FRACTIONAL_FRAME_TIME_TEXT)),
               DataFileValueOutOfRangeException);
}

/**
 * Given a sheet whose sprite sheet is a number
 * When it is read
 * Then reading stops: the sprite sheet is a text
 */
TEST(EntityAppearances, RefusesASpriteSheetThatIsNoText) {
  EXPECT_THROW(static_cast<void>(
                   appearancesReading(SHEET_WITH_A_NUMBER_AS_ITS_SHEET_TEXT)),
               DataFileTextExpectedException);
}

/**
 * Given the sheets shipped in assets/
 * When the client reads them
 * Then each of the five types the server ships has an appearance
 */
TEST(EntityAppearances, EveryShippedTypeHasAnAppearance) {
  const EntityAppearances appearances = shippedAppearances();

  for (const std::string_view name : SHIPPED_SHEET_NAMES) {
    SCOPED_TRACE(name);
    EXPECT_NE(appearances.named(name), nullptr);
  }
}

/**
 * Given the sheets shipped in assets/
 * When each idle frame is laid on the sprite sheet it names
 * Then it lies inside that sheet and shows at least one visible pixel, and
 * the pixels just around it are all transparent, so it cuts no sprite and
 * shows no neighbour
 */
TEST(EntityAppearances, EveryShippedFrameShowsOneWholeSprite) {
  const EntityAppearances appearances = shippedAppearances();
  for (const std::string_view name : SHIPPED_SHEET_NAMES) {
    const EntityAppearance* const shipped = appearances.named(name);
    ASSERT_NE(shipped, nullptr);
    const sf::Image sheet = shippedSheet(shipped->sheet);
    for (const SpriteFrame& frame : shipped->idleFrames) {
      SCOPED_TRACE(name);
      SCOPED_TRACE(frame.left);

      ASSERT_TRUE(liesInside(frame, sheet.getSize()));
      EXPECT_TRUE(showsAVisiblePixel(sheet, frame));
      EXPECT_FALSE(hasAVisiblePixelAround(sheet, frame));
    }
  }
}

/**
 * Given the sheets shipped in assets/
 * When each frame of each attack is laid on the sprite sheet it names
 * Then it lies inside that sheet and shows at least one visible pixel, and
 * the pixels just around it are all transparent, as an idle frame
 */
TEST(EntityAppearances, EveryShippedAttackFrameShowsOneWholeSprite) {
  const EntityAppearances appearances = shippedAppearances();
  for (const std::string_view name : SHIPPED_SHEET_NAMES) {
    const EntityAppearance* const shipped = appearances.named(name);
    ASSERT_NE(shipped, nullptr);
    const sf::Image sheet = shippedSheet(shipped->sheet);
    for (const ActionAnimation& action : shipped->actions) {
      for (const SpriteFrame& frame : action.frames) {
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
 * When the frame time of each is read
 * Then it is the one assets/README.md lists for its sprite: 80 ms for the
 * ship, 250 ms for a Bydo, 60 ms for a missile of a player, 120 ms for a
 * missile of an enemy and for the Voltjelly, 250 ms for the Reaper, 200 ms
 * for the Drillback, 250 ms for the Bydomancer, 120 ms for its orb and 100 ms
 * for a shard
 */
TEST(EntityAppearances, ShippedFramesLastWhatTheSheetReadmeLists) {
  const std::array<std::pair<std::string_view, Duration>, 11> listed{{
      {SHIP_SHEET, SHIP_FRAME_TIME_LISTED},
      {DRIFTER_SHEET, BYDO_FRAME_TIME_LISTED},
      {WEAVER_SHEET, BYDO_FRAME_TIME_LISTED},
      {PLAYER_MISSILE_SHEET, PLAYER_MISSILE_FRAME_TIME_LISTED},
      {ENEMY_MISSILE_SHEET, ENEMY_MISSILE_FRAME_TIME_LISTED},
      {VOLTJELLY_SHEET, VOLTJELLY_FRAME_TIME_LISTED},
      {REAPER_SHEET, REAPER_FRAME_TIME_LISTED},
      {DRILLBACK_SHEET, DRILLBACK_FRAME_TIME_LISTED},
      {BYDOMANCER_SHEET, BYDOMANCER_FRAME_TIME_LISTED},
      {RUNE_ORB_SHEET, RUNE_ORB_FRAME_TIME_LISTED},
      {ORB_SHARD_SHEET, ORB_SHARD_FRAME_TIME_LISTED},
  }};
  const EntityAppearances appearances = shippedAppearances();
  for (const auto& [name, frameTime] : listed) {
    SCOPED_TRACE(name);
    const EntityAppearance* const shipped = appearances.named(name);
    ASSERT_NE(shipped, nullptr);

    EXPECT_EQ(shipped->idleFrameDuration, frameTime);
  }
}

/**
 * Given every pair of shipped sheets
 * When their idle frames are compared
 * Then two sheets either show the very same frames, as the drifter and the
 * weaver do today, or share no pixel of the sprite sheet they both draw from
 */
TEST(EntityAppearances, ShippedSheetsShareTheirFramesWholeOrNotAtAll) {
  const EntityAppearances appearances = shippedAppearances();
  for (const std::string_view first : SHIPPED_SHEET_NAMES) {
    for (const std::string_view second : SHIPPED_SHEET_NAMES) {
      const EntityAppearance* const one = appearances.named(first);
      const EntityAppearance* const other = appearances.named(second);
      ASSERT_NE(one, nullptr);
      ASSERT_NE(other, nullptr);
      if (first == second || one->sheet != other->sheet ||
          one->idleFrames == other->idleFrames) {
        continue;
      }
      SCOPED_TRACE(first);
      SCOPED_TRACE(second);

      EXPECT_FALSE(overlap(one->idleFrames.front(), other->idleFrames.front()));
    }
  }
}

/**
 * Given the shipped ship sheet
 * When its first idle frame is read
 * Then it is the ship of the first player color, whose row the pilot colors
 * of the client count down from (inPlayerColorRow())
 */
TEST(EntityAppearances, TheShippedShipStartsOnTheFirstPilotColorRow) {
  const EntityAppearances appearances = shippedAppearances();
  const EntityAppearance* const ship = appearances.named(SHIP_SHEET);
  ASSERT_NE(ship, nullptr);

  EXPECT_EQ(ship->idleFrames.front(), PLAYER_SHIP_FRAME);
}

/**
 * Given the shipped sheets, read by the client for their look and by the
 * server for their hitbox
 * When the idle frame of each is measured in logical units, each pixel of the
 * sheet covering BACKGROUND_PIXEL_SIZE
 * Then it is the box the server hits that type with, for every sheet but the
 * Voltjelly, whose frame also holds its aura
 */
TEST(EntityAppearances, EveryShippedSheetDrawsTheBoxTheServerHits) {
  const EntityAppearances appearances = shippedAppearances();
  for (const EntitySheet& sheet : shippedEntitySheets().all()) {
    if (sheet.name == VOLTJELLY_SHEET) {
      continue;
    }
    SCOPED_TRACE(sheet.name);
    const EntityAppearance* const look = appearances.named(sheet.name);
    ASSERT_NE(look, nullptr);
    const SpriteFrame frame = look->idleFrames.front();

    EXPECT_FLOAT_EQ(static_cast<float>(frame.width) * BACKGROUND_PIXEL_SIZE,
                    sheet.hitbox.width);
    EXPECT_FLOAT_EQ(static_cast<float>(frame.height) * BACKGROUND_PIXEL_SIZE,
                    sheet.hitbox.height);
  }
}

/**
 * Given the shipped sheet of the Voltjelly, whose aura hurts but lets a shot
 * through to its bell
 * When its idle frame and its hitbox are measured, each pixel of the sheet
 * covering BACKGROUND_PIXEL_SIZE
 * Then the frame is 40 by 40 pixels and the hitbox is the bell of 24 by 24
 * pixels at its center
 */
TEST(EntityAppearances, TheVoltjellyDrawsItsAuraAroundTheBoxTheServerHits) {
  const EntityAppearances appearances = shippedAppearances();
  const EntityAppearance* const look = appearances.named(VOLTJELLY_SHEET);
  ASSERT_NE(look, nullptr);
  const EntitySheet& sheet = shippedEntitySheets().named(VOLTJELLY_SHEET);

  EXPECT_EQ(look->idleFrames.front().width, VOLTJELLY_FRAME_SIDE);
  EXPECT_EQ(look->idleFrames.front().height, VOLTJELLY_FRAME_SIDE);
  EXPECT_FLOAT_EQ(sheet.hitbox.width, static_cast<float>(VOLTJELLY_BELL_SIDE) *
                                          BACKGROUND_PIXEL_SIZE);
  EXPECT_FLOAT_EQ(sheet.hitbox.height, static_cast<float>(VOLTJELLY_BELL_SIDE) *
                                           BACKGROUND_PIXEL_SIZE);
}
