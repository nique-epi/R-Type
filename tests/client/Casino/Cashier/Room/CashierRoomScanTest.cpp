#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Graphics/Image.hpp>
#include "CashierRoom.hpp"
#include "CashierRoomConstants.hpp"
#include "CashierRoomScan.hpp"
#include "CashierRoomScanTestConstants.hpp"
#include "CasinoSpriteConstants.hpp"
#include "ColoredPixel.hpp"
#include "SheetPixels.hpp"

using rtype::client::CASHIER_GRILLE_AREA;
using rtype::client::CASHIER_ROOM_ID;
using rtype::client::CashierRoom;
using rtype::client::cashierRoomOf;
using rtype::client::ColoredPixel;
using rtype::client::testing::CAGE_TOO_SMALL_A_PICTURE;
using ::testing::IsEmpty;
using ::testing::Not;

/**
 * Given the picture of the cage shipped in assets/
 * When the cashier reads it
 * Then it finds gold in the vault for its coins to sparkle
 */
TEST(CashierRoomScan, FindsTheGoldOfTheVault) {
  const CashierRoom room = cashierRoomOf(shippedSheet(CASHIER_ROOM_ID));

  EXPECT_THAT(room.gold, Not(IsEmpty()));
}

/**
 * Given the picture of the cage shipped in assets/
 * When the cashier reads it
 * Then it keeps the brass of the grille, inside the grille only
 */
TEST(CashierRoomScan, KeepsTheBrassOfTheGrille) {
  const CashierRoom room = cashierRoomOf(shippedSheet(CASHIER_ROOM_ID));

  ASSERT_THAT(room.grille, Not(IsEmpty()));
  for (const ColoredPixel& brass : room.grille) {
    EXPECT_TRUE(CASHIER_GRILLE_AREA.contains(brass.position));
  }
}

/**
 * Given a picture smaller than the interface
 * When the cashier reads it
 * Then it finds nothing, and reads no pixel outside the picture
 */
TEST(CashierRoomScan, APictureOfAnotherSizeGivesAStillCage) {
  const CashierRoom room = cashierRoomOf(sf::Image(CAGE_TOO_SMALL_A_PICTURE));

  EXPECT_THAT(room.gold, IsEmpty());
  EXPECT_THAT(room.grille, IsEmpty());
}
