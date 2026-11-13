#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Window/Keyboard.hpp>
#include <optional>
#include <string>
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "ShopScreenConstants.hpp"
#include "ShopScreenFixture.hpp"
#include "ShopScreenTestConstants.hpp"

using rtype::client::PURCHASE_SOUND_ID;
using rtype::client::testing::A_FORTUNE;
using rtype::client::testing::A_FORTUNE_LESS_THE_PARTY_HAT;
using rtype::client::testing::ALREADY_YOURS_LINE;
using rtype::client::testing::BOUGHT_PARTY_HAT_LINE;
using rtype::client::testing::CHEAPEST_HULL_ITEM;
using rtype::client::testing::CROWN_ITEM;
using rtype::client::testing::FIRST_STALL_MIDDLE_OF_THE_SHOP;
using rtype::client::testing::ONE_KOPECK_MISSING_LINE;
using rtype::client::testing::ONE_KOPECK_UNDER_THE_PARTY_HAT;
using rtype::client::testing::OWNED_PARTY_HAT_LINE;
using rtype::client::testing::PARTY_HAT_FOR_SALE_LINE;
using rtype::client::testing::PARTY_HAT_ITEM;
using rtype::client::testing::SECOND_CELL_MIDDLE;
using rtype::client::testing::SHOP_SCENE_SHEET;
using rtype::client::testing::ShopScreenFixture;
using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::Not;
using Key = sf::Keyboard::Key;

/**
 * Given a shop just opened, the party hat chosen
 * When it is painted
 * Then the picture of the booth is laid first, and the status line names the
 * party hat and its price
 */
TEST_F(ShopScreenFixture, OpensOnThePictureAndTheFirstWare) {
  ASSERT_FALSE(painting().images.empty());
  EXPECT_EQ(painting().images.front().assetId, SHOP_SCENE_SHEET);
  EXPECT_NE(writingOf(painting(), PARTY_HAT_FOR_SALE_LINE), nullptr);
}

/**
 * Given a rich player, the party hat chosen
 * When Enter is pressed
 * Then the locker holds the party hat
 */
TEST_F(ShopScreenFixture, EnterPutsTheChosenLookInTheLocker) {
  give(A_FORTUNE);

  press(Key::Enter);

  EXPECT_TRUE(settings().shipLocker.items.test(PARTY_HAT_ITEM));
}

/**
 * Given a rich player, the party hat chosen
 * When Enter is pressed
 * Then 150 kopecks leave the balance, and the settings are saved once with
 * what is left
 */
TEST_F(ShopScreenFixture, APurchaseTakesItsPriceAndIsSaved) {
  give(A_FORTUNE);

  press(Key::Enter);

  ASSERT_EQ(storage().saves().size(), 1U);
  EXPECT_EQ(storage().saves().back().kopecks, A_FORTUNE_LESS_THE_PARTY_HAT);
}

/**
 * Given a rich player, the party hat chosen
 * When Enter is pressed
 * Then the sound of a purchase plays and the status line names the look
 */
TEST_F(ShopScreenFixture, APurchaseIsHeardAndNamed) {
  give(A_FORTUNE);

  press(Key::Enter);

  EXPECT_THAT(sounds().played(), Contains(std::string{PURCHASE_SOUND_ID}));
  EXPECT_NE(writingOf(painting(), BOUGHT_PARTY_HAT_LINE), nullptr);
}

/**
 * Given a player one kopeck short of the party hat
 * When Enter is pressed
 * Then the status line says one kopeck is missing
 */
TEST_F(ShopScreenFixture, APoorPlayerIsToldWhatIsMissing) {
  give(ONE_KOPECK_UNDER_THE_PARTY_HAT);

  press(Key::Enter);

  EXPECT_NE(writingOf(painting(), ONE_KOPECK_MISSING_LINE), nullptr);
}

/**
 * Given a player one kopeck short of the party hat
 * When Enter is pressed
 * Then nothing is bought, saved or heard as a purchase
 */
TEST_F(ShopScreenFixture, ARefusedPurchaseChangesNothing) {
  give(ONE_KOPECK_UNDER_THE_PARTY_HAT);

  press(Key::Enter);

  EXPECT_FALSE(settings().shipLocker.items.test(PARTY_HAT_ITEM));
  EXPECT_EQ(settings().kopecks, ONE_KOPECK_UNDER_THE_PARTY_HAT);
  EXPECT_TRUE(storage().saves().empty());
  EXPECT_THAT(sounds().played(), Not(Contains(std::string{PURCHASE_SOUND_ID})));
}

/**
 * Given a rich player, the party hat chosen
 * When Enter is held, its press repeated before any release
 * Then the party hat is bought once: the line still says it was bought, not
 * that it is already owned
 */
TEST_F(ShopScreenFixture, AHeldEnterBuysOnce) {
  give(A_FORTUNE);
  hold(Key::Enter);

  hold(Key::Enter);

  EXPECT_NE(writingOf(painting(), BOUGHT_PARTY_HAT_LINE), nullptr);
}

/**
 * Given a player who just bought the party hat
 * When Enter is released, then pressed again on it
 * Then the status line says it is already theirs
 */
TEST_F(ShopScreenFixture, BuyingAnOwnedLookSaysItIsAlreadyYours) {
  give(A_FORTUNE);
  press(Key::Enter);

  press(Key::Enter);

  EXPECT_NE(writingOf(painting(), ALREADY_YOURS_LINE), nullptr);
}

/**
 * Given a player who bought the party hat, then chose its neighbour
 * When the party hat is chosen again
 * Then its status line says it is already theirs, and names no price
 */
TEST_F(ShopScreenFixture, AnOwnedLookShowsNoPriceOnItsLine) {
  give(A_FORTUNE);
  press(Key::Enter);
  press(Key::Right);

  press(Key::Left);

  EXPECT_NE(writingOf(painting(), OWNED_PARTY_HAT_LINE), nullptr);
}

/**
 * Given a rich player, on a window whose pixels are not those of the
 * interface
 * When the second cell of the locker is clicked
 * Then the crown, the look of that cell, is bought
 */
TEST_F(ShopScreenFixture, AClickBuysTheLookUnderIt) {
  give(A_FORTUNE);

  pointAt(SECOND_CELL_MIDDLE, true);

  EXPECT_TRUE(settings().shipLocker.items.test(CROWN_ITEM));
}

/**
 * Given a rich player
 * When the pointer moves over the second cell, then Enter is pressed
 * Then the crown is bought: the pointer chose it
 */
TEST_F(ShopScreenFixture, ThePointerChoosesTheLookItMovesOver) {
  give(A_FORTUNE);
  pointAt(SECOND_CELL_MIDDLE, false);

  press(Key::Enter);

  EXPECT_TRUE(settings().shipLocker.items.test(CROWN_ITEM));
}

/**
 * Given a rich player
 * When the first stall of the glass case is clicked
 * Then the cheapest hull is bought
 */
TEST_F(ShopScreenFixture, AClickOnAStallBuysItsHull) {
  give(A_FORTUNE);

  pointAt(FIRST_STALL_MIDDLE_OF_THE_SHOP, true);

  EXPECT_TRUE(settings().shipLocker.items.test(CHEAPEST_HULL_ITEM));
}

/**
 * Given a shop
 * When Escape is pressed and released
 * Then the shop asks to be closed
 */
TEST_F(ShopScreenFixture, EscapeClosesTheShop) {
  press(Key::Escape);

  EXPECT_THAT(navigator().calls(),
              ElementsAre(NavigatorCall{.request = NavigatorRequest::Pop,
                                        .screen = std::nullopt}));
}
