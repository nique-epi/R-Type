#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <vector>
#include "RecordingInterfaceCanvas.hpp"
#include "ShipLook.hpp"
#include "ShipLooks.hpp"
#include "ShipLooksTestConstants.hpp"

using rtype::client::paintShipLook;
using rtype::client::wearing;
using rtype::client::wears;
using rtype::client::without;
using rtype::client::wornItemsOf;
using rtype::client::testing::COOL_SHADES;
using rtype::client::testing::FULLY_DRESSED;
using rtype::client::testing::LOOK_SHEET;
using rtype::client::testing::MUSTACHE;
using rtype::client::testing::PARTY_HAT;
using rtype::client::testing::SHIP_CORNER;
using rtype::client::testing::TOP_HAT;
using rtype::client::testing::TOP_HAT_CORNER;
using rtype::client::testing::TWICE;
using rtype::client::testing::WEARS_NOTHING;
using rtype::client::testing::WEARS_PARTY_HAT;
using rtype::client::testing::WEARS_SHADES;
using rtype::client::testing::WEARS_TOP_HAT;
using rtype::identifiers::ShipLook;
using ::testing::ElementsAre;

/**
 * Given a ship wearing the top hat
 * When it wears the party hat
 * Then the party hat takes the head, in place of the top hat
 */
TEST(ShipLooks, AHatReplacesTheHatOfTheHead) {
  const ShipLook look = wearing(ShipLook{.head = WEARS_TOP_HAT}, PARTY_HAT);

  EXPECT_EQ(look.head, WEARS_PARTY_HAT);
}

/**
 * Given a ship wearing the top hat
 * When it wears the shades
 * Then it keeps the hat and wears the shades on its eyes
 */
TEST(ShipLooks, ShadesKeepTheHat) {
  const ShipLook look = wearing(ShipLook{.head = WEARS_TOP_HAT}, COOL_SHADES);

  EXPECT_EQ(look.head, WEARS_TOP_HAT);
  EXPECT_EQ(look.eyes, WEARS_SHADES);
}

/**
 * Given a ship wearing the party hat
 * When the top hat is taken off
 * Then the party hat stays
 */
TEST(ShipLooks, TakingOffAnotherItemOfTheSlotChangesNothing) {
  EXPECT_EQ(without(ShipLook{.head = WEARS_PARTY_HAT}, TOP_HAT).head,
            WEARS_PARTY_HAT);
}

/**
 * Given a ship wearing the top hat
 * When the top hat is taken off
 * Then the head wears nothing
 */
TEST(ShipLooks, TakingOffTheWornItemEmptiesItsSlot) {
  EXPECT_EQ(without(ShipLook{.head = WEARS_TOP_HAT}, TOP_HAT).head,
            WEARS_NOTHING);
}

/**
 * Given a ship wearing the shades
 * When whether it wears them is asked
 * Then it does, and it wears no top hat
 */
TEST(ShipLooks, WearsTellsTheItemOfItsSlot) {
  const ShipLook look{.eyes = WEARS_SHADES};

  EXPECT_TRUE(wears(look, COOL_SHADES));
  EXPECT_FALSE(wears(look, TOP_HAT));
}

/**
 * Given a ship wearing an item in every slot
 * When its worn items are listed
 * Then they come in the order of the locker: hat, shades, mustache
 */
TEST(ShipLooks, WornItemsComeInTheOrderOfTheLocker) {
  EXPECT_THAT(wornItemsOf(FULLY_DRESSED),
              ElementsAre(TOP_HAT, COOL_SHADES, MUSTACHE));
}

/**
 * Given a ship drawn at (100, 100) twice its size, wearing the top hat
 * When its look is painted
 * Then the top hat is drawn once, from (118, 86), twice its size
 */
TEST(ShipLooks, TheTopHatSitsOnTheTopFin) {
  InterfaceCanvasRecord record;
  RecordingInterfaceCanvas canvas(record);

  paintShipLook(canvas, ShipLook{.head = WEARS_TOP_HAT},
                {.topLeft = SHIP_CORNER, .scale = TWICE});

  ASSERT_EQ(record.images.size(), 1U);
  EXPECT_EQ(record.images.front().assetId, LOOK_SHEET);
  EXPECT_EQ(record.images.front().topLeft, TOP_HAT_CORNER);
  EXPECT_EQ(record.images.front().scale, TWICE);
}
