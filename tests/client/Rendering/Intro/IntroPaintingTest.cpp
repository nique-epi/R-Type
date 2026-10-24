#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <algorithm>
#include "EffectColorConstants.hpp"
#include "EffectConstants.hpp"
#include "EntitySpriteConstants.hpp"
#include "FlashPreference.hpp"
#include "InterfaceColorConstants.hpp"
#include "IntroPaintingFixtures.hpp"
#include "IntroTestConstants.hpp"
#include "IntroTimeline.hpp"
#include "IntroTimelineConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "SquadronTestConstants.hpp"

using rtype::client::BYDO_EXPLOSION_FRAME;
using rtype::client::BYDO_EXPLOSION_WITHOUT_FLASH_FRAME;
using rtype::client::BYDO_LILAC;
using rtype::client::closeupStartOf;
using rtype::client::EFFECT_SHEET_ID;
using rtype::client::ENTITY_SHEET_ID;
using rtype::client::FlashPreference;
using rtype::client::INTRO_FIRST_KILL;
using rtype::client::INTRO_SCENE_BOOST;
using rtype::client::INTRO_SWARM_CUT;
using rtype::client::INTRO_TITLE;
using rtype::client::PLAYER_BLUE;
using rtype::client::PLAYER_SHIP_FRAME;
using rtype::client::testing::A_MOMENT;
using rtype::client::testing::CLOSEUP_SCALE;
using rtype::client::testing::FULL_SPEED_LINES;
using rtype::client::testing::SHIP_SIZE;
using rtype::client::testing::SHIPS_IN_PLACE_ON_THE_TITLE;
using rtype::client::testing::WIDE_SCALE;
using ::testing::UnorderedElementsAreArray;

/**
 * Given the close-up of the first pilot
 * When it is painted
 * Then its ship is drawn three times larger
 */
TEST(IntroPainting, ACloseUpDrawsItsShipThreeTimesLarger) {
  const InterfaceCanvasRecord record =
      paintedAt(closeupStartOf(0) + A_MOMENT, FlashPreference::Full);

  EXPECT_TRUE(
      std::ranges::any_of(record.images, [](const RecordedImage& image) {
        return image.assetId == ENTITY_SHEET_ID &&
               image.source.position.y == PLAYER_SHIP_FRAME.top &&
               image.scale == CLOSEUP_SCALE;
      }));
}

/**
 * Given the wide shot of the swarm
 * When it is painted
 * Then every image is drawn at its own size
 */
TEST(IntroPainting, TheWideShotDrawsAtNativeSize) {
  const InterfaceCanvasRecord record =
      paintedAt(INTRO_SWARM_CUT + A_MOMENT, FlashPreference::Full);

  EXPECT_TRUE(std::ranges::all_of(
      record.images,
      [](const RecordedImage& image) { return image.scale == WIDE_SCALE; }));
}

/**
 * Given the first kill of the cascade, with flashes
 * When it is painted just after the hit
 * Then the burst of the Bydo shows its standard row
 */
TEST(IntroPainting, AKillShowsItsStandardBurstWithFlashes) {
  const InterfaceCanvasRecord record = paintedAt(
      INTRO_FIRST_KILL + rtype::client::INTRO_FIRST_IMPACT_FREEZE + A_MOMENT,
      FlashPreference::Full);

  EXPECT_GT(imagesOnRow(record, EFFECT_SHEET_ID, BYDO_EXPLOSION_FRAME.top), 0U);
}

/**
 * Given the first kill of the cascade, with REDUCE FLASHES
 * When it is painted just after the hit
 * Then the burst of the Bydo shows its row without flash, never the standard
 * one
 */
TEST(IntroPainting, WithReducedFlashesAKillShowsNoFlash) {
  const InterfaceCanvasRecord record =
      paintedAt(INTRO_FIRST_KILL + A_MOMENT, FlashPreference::Reduced);

  EXPECT_GT(imagesOnRow(record, EFFECT_SHEET_ID,
                        BYDO_EXPLOSION_WITHOUT_FLASH_FRAME.top),
            0U);
  EXPECT_EQ(imagesOnRow(record, EFFECT_SHEET_ID, BYDO_EXPLOSION_FRAME.top), 0U);
}

/**
 * Given the first Bydo of the helix just before its missile hits it
 * When the scene is painted
 * Then it is drawn tinted lilac
 */
TEST(IntroPainting, ABydoAboutToBeHitIsTintedLilac) {
  const InterfaceCanvasRecord record =
      paintedAt(INTRO_FIRST_KILL - A_MOMENT, FlashPreference::Full);

  EXPECT_GT(imagesTinted(record, BYDO_LILAC), 0U);
}

/**
 * Given the first ship entering the wide shot
 * When it is painted
 * Then its nearest ghost is drawn in its color
 */
TEST(IntroPainting, AnEnteringShipLeavesGhostsInItsColor) {
  const InterfaceCanvasRecord record =
      paintedAt(INTRO_SWARM_CUT + A_MOMENT, FlashPreference::Full);

  EXPECT_GT(imagesTinted(record, PLAYER_BLUE), 0U);
}

/**
 * Given the warp of the boost, with flashes
 * When it is painted
 * Then forty speed lines cross the screen
 */
TEST(IntroPainting, TheWarpDrawsFortySpeedLines) {
  const InterfaceCanvasRecord record =
      paintedAt(INTRO_SCENE_BOOST + rtype::client::INTRO_FREEZES_LENGTH +
                    rtype::client::INTRO_WARP_RISE + A_MOMENT,
                FlashPreference::Full);

  EXPECT_EQ(speedLinesOf(record), FULL_SPEED_LINES);
}

/**
 * Given the warp of the boost, with REDUCE FLASHES
 * When it is painted
 * Then no speed line is drawn
 */
TEST(IntroPainting, WithReducedFlashesTheWarpDrawsNoSpeedLine) {
  const InterfaceCanvasRecord record =
      paintedAt(INTRO_SCENE_BOOST + rtype::client::INTRO_WARP_RISE + A_MOMENT,
                FlashPreference::Reduced);

  EXPECT_EQ(speedLinesOf(record), 0U);
}

/**
 * Given the ships landed in the squadron of the title
 * When the scene is painted just before the title opens
 * Then the four ships lie where the title shows its squadron in place
 */
TEST(IntroPainting, TheLandedShipsLieWhereTheTitleShowsThem) {
  const InterfaceCanvasRecord record =
      paintedAt(INTRO_TITLE - A_MOMENT, FlashPreference::Full);

  EXPECT_THAT(untintedImagesOf(record, ENTITY_SHEET_ID, SHIP_SIZE),
              UnorderedElementsAreArray(SHIPS_IN_PLACE_ON_THE_TITLE));
}
