#include <gtest/gtest.h>
#include "EntitySpriteConstants.hpp"
#include "PilotHull.hpp"
#include "ShipHullSpriteConstants.hpp"
#include "ShipHulls.hpp"
#include "ShipHullsTestConstants.hpp"
#include "ShipSheetFrame.hpp"

using rtype::client::ENTITY_SHEET_ID;
using rtype::client::hullFramesOf;
using rtype::client::PilotHull;
using rtype::client::pilotShipFrameOf;
using rtype::client::SHIP_HULL_SHEET_ID;
using rtype::client::ShipSheetFrame;
using rtype::client::testing::CROWN_NUMBER_AS_A_HULL;
using rtype::client::testing::GOLDEN_BYDO_HULL;
using rtype::client::testing::GOLDEN_BYDO_THIRD_COLOR_TOP;
using rtype::client::testing::NO_PILOT_COLOR;
using rtype::client::testing::PAST_THE_CATALOG;
using rtype::client::testing::SECOND_SHIP_FRAME;
using rtype::client::testing::STANDARD_HULL;
using rtype::client::testing::THIRD_COLOR;
using rtype::client::testing::THIRD_COLOR_TOP;
using rtype::client::testing::WAR_SHARK_HULL;

/**
 * Given a pilot of the third color flying the standard hull
 * When the picture of the second frame of its ship is asked
 * Then it is that frame in the third row of the entity sheet
 */
TEST(ShipHulls, TheStandardHullIsARowOfTheEntitySheet) {
  const ShipSheetFrame shown =
      pilotShipFrameOf(PilotHull{.hull = STANDARD_HULL, .color = THIRD_COLOR},
                       SECOND_SHIP_FRAME);

  EXPECT_EQ(shown.sheet, ENTITY_SHEET_ID);
  EXPECT_EQ(shown.frame.top, THIRD_COLOR_TOP);
  EXPECT_EQ(shown.frame.left, SECOND_SHIP_FRAME.left);
}

/**
 * Given a pilot of the third color flying the first hull
 * When the picture of the second frame of its ship is asked
 * Then it is that frame in the third row of the first block of the hull sheet
 */
TEST(ShipHulls, AHullIsARowOfItsBlockOfTheHullSheet) {
  const ShipSheetFrame shown =
      pilotShipFrameOf(PilotHull{.hull = WAR_SHARK_HULL, .color = THIRD_COLOR},
                       SECOND_SHIP_FRAME);

  EXPECT_EQ(shown.sheet, SHIP_HULL_SHEET_ID);
  EXPECT_EQ(shown.frame.top, THIRD_COLOR_TOP);
  EXPECT_EQ(shown.frame.left, SECOND_SHIP_FRAME.left);
}

/**
 * Given a pilot of the third color flying the last hull
 * When the picture of its ship is asked
 * Then it lies in the fifth block, four blocks of four rows down
 */
TEST(ShipHulls, TheLastHullLiesInTheLastBlock) {
  const ShipSheetFrame shown = pilotShipFrameOf(
      PilotHull{.hull = GOLDEN_BYDO_HULL, .color = THIRD_COLOR},
      SECOND_SHIP_FRAME);

  EXPECT_EQ(shown.frame.top, GOLDEN_BYDO_THIRD_COLOR_TOP);
}

/**
 * Given a hull byte that names an accessory, then one past the catalog
 * When the picture of the ship is asked
 * Then it is the standard hull both times
 */
TEST(ShipHulls, ANumberThatIsNoHullGivesTheStandardHull) {
  EXPECT_EQ(pilotShipFrameOf(
                PilotHull{.hull = CROWN_NUMBER_AS_A_HULL, .color = THIRD_COLOR},
                SECOND_SHIP_FRAME)
                .sheet,
            ENTITY_SHEET_ID);
  EXPECT_EQ(pilotShipFrameOf(
                PilotHull{.hull = PAST_THE_CATALOG, .color = THIRD_COLOR},
                SECOND_SHIP_FRAME)
                .sheet,
            ENTITY_SHEET_ID);
}

/**
 * Given the standard hull, then a hull flown by a color no pilot has
 * When the frames of each hull are asked
 * Then there are none: the entity sheet gives the frames of the ship
 */
TEST(ShipHulls, OnlyAHullOfAPilotColorHasHullFrames) {
  EXPECT_TRUE(
      hullFramesOf(PilotHull{.hull = STANDARD_HULL, .color = THIRD_COLOR})
          .empty());
  EXPECT_TRUE(
      hullFramesOf(PilotHull{.hull = WAR_SHARK_HULL, .color = NO_PILOT_COLOR})
          .empty());
  EXPECT_FALSE(
      hullFramesOf(PilotHull{.hull = WAR_SHARK_HULL, .color = THIRD_COLOR})
          .empty());
}
